/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-24 23:04:15
*/

#include "Light/DirectionnalLight.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Config.hpp"
#include "Framebuffer.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Render.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Global.hpp"
#include "Shader/Program.hpp"

#include "SphericalHarmonics.hpp"

#include <glm/glm.hpp>

static inline auto DirectionnalLightGeometry()
{
    static auto geometry = CubeMesh::CreateGeometry("DirectionnalLightGeometry", glm::vec3(1));
    return geometry;
}

static inline auto DirectionnalLightVertexCode()
{
    static auto lightingVertexCode =
#include "light.vert"
        ;
    static Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DirectionnalLightFragmentCode()
{
    static auto deferred_frag_code =
#include "deferred.frag"
        ;
    static auto randomCode =
#include "Random.glsl"
        ;
    static auto shadowCode =
#include "SampleShadowMap.glsl"
        ;
    static auto lightingFragmentCode =
#include "directionnalLight.frag"
        ;
    static Shader::Stage::Code shaderCode =
        Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code { randomCode } +
        Shader::Stage::Code { shadowCode } +
        Shader::Stage::Code { lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

auto DirectionnalLightShader()
{
    static std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        shader = Component::Create<Shader::Program>("DirectionnalLightShader");
        shader->SetDefine("LIGHTSHADER");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DirectionnalLightVertexCode()));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DirectionnalLightFragmentCode()));
    }
    return shader;
}

auto DirectionnalLightLUTShader() {
    static std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        auto deferredVertexCode = 
#include "deferred.vert"
            ;
        auto dirLightFragCode =
#include "dirLightDiffuseLUT.frag"
            ;
        shader = Component::Create<Shader::Program>("DirectionnalLUTShader");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { dirLightFragCode, "Lighting();" }));
    }
    return shader;
}

DirectionnalLight::DirectionnalLight()
{
    static auto g_dirLightNbr = 0u;
    SetName("DirectionalLight_" + std::to_string(g_dirLightNbr));
    ++g_dirLightNbr;
}

DirectionnalLight::DirectionnalLight(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow)
    : DirectionnalLight()
{
    SetName(name);
    SetColor(color);
    SetDirection(direction);
    SetHalfSize(glm::vec3(500));
    SetComponent(DirectionnalLightShader());
    SetCastShadow(cast_shadow);
    if (cast_shadow) {
        _SetShadowBuffer(Component::Create<Framebuffer>(GetName() + "_shadowMap", glm::vec2(Config::Global().Get("ShadowRes", 1024)), 0, 0));
        _GetShadowBuffer()->Create_attachement(Pixel::SizedFormat::Depth24);
        _GetShadowBuffer()->depth()->SetParameter<Texture::Parameter::CompareMode>(Texture::CompareMode::CompareRefToTexture);
        _GetShadowBuffer()->depth()->SetParameter<Texture::Parameter::CompareFunc>(Texture::CompareFunc::LessEqual);
    }
}

glm::vec3 DirectionnalLight::GetDirection() const
{
    return _direction;
}

void DirectionnalLight::SetDirection(const glm::vec3& direction)
{
    _direction = normalize(direction);
}
glm::vec3 DirectionnalLight::HalfSize() const
{
    return GetScale() / 2.f;
}
void DirectionnalLight::SetHalfSize(const glm::vec3& halfSize)
{
    SetScale(halfSize * 2.f);
}
glm::vec3 DirectionnalLight::Min() const
{
    return WorldPosition() - HalfSize();
}
glm::vec3 DirectionnalLight::Max() const
{
    return WorldPosition() + HalfSize();
}

bool DirectionnalLight::Infinite() const
{
    return _infinite;
}

void DirectionnalLight::SetInfinite(bool infinite)
{
    _infinite = infinite;
}

auto GetUp(const glm::vec3& direction)
{
    auto up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(normalize(direction), up)) > .9999f)
        up = glm::vec3(1, 0, 0);
    return up;
}

std::array<glm::vec3, 8> ExtractFrustum(const std::shared_ptr<Camera>& camera)
{
    std::array<glm::vec3, 8> NDCCube {
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f)
    };
    auto invVP = glm::inverse(camera->ProjectionMatrix() * camera->ViewMatrix());
    for (auto& v : NDCCube) {
        glm::vec4 normalizedCoord = invVP * glm::vec4(v, 1);
        v = glm::vec3(normalizedCoord) / normalizedCoord.w;
    }
    return NDCCube;
}

void DirectionnalLight::render_shadow()
{
    if (GetCastShadow())
        Infinite() ? DrawShadowInfinite() : DrawShadowFinite();
}

void NormalizePlane(glm::vec4& plane)
{
    plane /= glm::length(glm::vec3(plane));
}

std::array<glm::vec4, 6> ExtractPlanes(const glm::mat4& matrix, bool normalizePlanes = false)
{
    std::array<glm::vec4, 6> planes;
    glm::vec4 rowX = glm::row(matrix, 0);
    glm::vec4 rowY = glm::row(matrix, 1);
    glm::vec4 rowZ = glm::row(matrix, 2);
    glm::vec4 rowW = glm::row(matrix, 3);

    planes[0] = normalize(rowW + rowX);
    planes[1] = normalize(rowW - rowX);
    planes[2] = normalize(rowW + rowY);
    planes[3] = normalize(rowW - rowY);
    planes[4] = normalize(rowW + rowZ);
    planes[5] = normalize(rowW - rowZ);
    if (normalizePlanes) {
        NormalizePlane(planes[0]);
        NormalizePlane(planes[1]);
        NormalizePlane(planes[2]);
        NormalizePlane(planes[3]);
        NormalizePlane(planes[4]);
        NormalizePlane(planes[5]);
    }
    std::cout << std::endl;
    std::cout << planes[0].x << ' ' << planes[0].y << ' ' << planes[0].z << ' ' << planes[0].w << std::endl;
    std::cout << planes[1].x << ' ' << planes[1].y << ' ' << planes[1].z << ' ' << planes[1].w << std::endl;
    std::cout << planes[2].x << ' ' << planes[2].y << ' ' << planes[2].z << ' ' << planes[2].w << std::endl;
    std::cout << planes[3].x << ' ' << planes[3].y << ' ' << planes[3].z << ' ' << planes[3].w << std::endl;
    std::cout << planes[4].x << ' ' << planes[4].y << ' ' << planes[4].z << ' ' << planes[4].w << std::endl;
    std::cout << planes[5].x << ' ' << planes[5].y << ' ' << planes[5].z << ' ' << planes[5].w << std::endl;
    std::cout << std::endl;
    return planes;
}

void DirectionnalLight::Draw()
{
    static auto SH{ SphericalHarmonics(100) };
    static auto sphericalHarmonicsColor{
        SH.ProjectFunction(
        [=](const SphericalHarmonics::Sample& sample) {
                return GetColor() * glm::dot(sample.vec, glm::normalize(GetDirection()));
        })
    };
    if (_diffuseLUTNeedsUpdate) {
        if (_specularLUT == nullptr) {
            _specularLUT = Component::Create<Texture2D>(glm::ivec2(256), Pixel::SizedFormat::Float16_RGB);
            _specularLUT->SetParameter<Texture::Parameter::WrapS>(Texture::Wrap::ClampToEdge);
            _specularLUT->SetParameter<Texture::Parameter::WrapT>(Texture::Wrap::ClampToEdge);
            _specularLUT->Load();
            _specularLUT->GenerateMipmap();
        }
        static auto s_diffuseLUTBuffer = Component::Create<Framebuffer>("DiffuseLUTBuffer", glm::ivec2(256));
        glDisable(GL_CULL_FACE);
        s_diffuseLUTBuffer->set_attachement(0, nullptr);
        s_diffuseLUTBuffer->Resize(glm::ivec2(256));
        for (auto level = 0u; level < _specularLUT->GetMipMapNbr(); ++level) {
            s_diffuseLUTBuffer->set_attachement(0, _specularLUT, level);
            s_diffuseLUTBuffer->bind();
            float BRDFAlpha{ level / float(_specularLUT->GetMipMapNbr()) };
            DirectionnalLightLUTShader()->Use()
                .SetUniform("SpecularMode", true)
                .SetUniform("BRDFAlpha", BRDFAlpha);
            glClear(GL_COLOR_BUFFER_BIT);
            Render::DisplayQuad()->Draw();
            s_diffuseLUTBuffer->set_attachement(0, nullptr);
            s_diffuseLUTBuffer->Resize(s_diffuseLUTBuffer->Size() / 2);
        }
        _diffuseLUTNeedsUpdate = false;
        //re-bind lighting buffer
        Render::LightBuffer()->bind();
        glEnable(GL_CULL_FACE);
    }

    auto geometryBuffer = Render::GeometryBuffer();
    glm::vec3 geometryPosition;
    if (Infinite())
        geometryPosition = Scene::Current()->CurrentCamera()->WorldPosition();
    else
        geometryPosition = GetParent() ? GetParent()->WorldPosition() + GetPosition() : GetPosition();
    if (GetCastShadow())
        DirectionnalLightShader()->SetDefine("SHADOW");
    else
        DirectionnalLightShader()->RemoveDefine("SHADOW");
    DirectionnalLightShader()->Use()
        .SetTexture("SpecularLUT", _specularLUT)
        .SetTexture("DefaultBRDFLUT", Render::DefaultBRDFLUT())
        .SetTexture("Light.Shadow", GetCastShadow() ? _GetShadowBuffer()->depth() : nullptr)
        .SetUniform("SphericalHarmonics[0]", sphericalHarmonicsColor.data(), sphericalHarmonicsColor.size())
        .SetUniform("SHCartesianShapes[0]", SH.GetCartesianCoeffs().data(), SH.GetCartesianCoeffs().size())
        .SetUniform("Light.Max", Max())
        .SetUniform("Light.Min", Min())
        .SetUniform("Light.Projection", (Infinite() ? ShadowProjectionMatrixInfinite() : ShadowProjectionMatrixFinite()) * ShadowViewMatrix())
        .SetUniform("Light.Color", GetColor())
        .SetUniform("Light.Direction", GetDirection())
        .SetUniform("Light.Infinite", Infinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->attachement(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->attachement(2))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->attachement(4))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->depth());
    DirectionnalLightGeometry()->Draw();
    DirectionnalLightShader()->Done();
}

void DirectionnalLight::DrawShadowInfinite()
{
    auto camera = Scene::Current()->CurrentCamera();
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - GetDirection() * radius;

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", ShadowViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", ShadowProjectionMatrixInfinite());
    _shadowBuffer->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->RenderDepth(Render::Mode::All);
    _shadowBuffer->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
}

void DirectionnalLight::DrawShadowFinite()
{
    auto camera = Scene::Current()->CurrentCamera();
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - GetDirection() * radius;

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", ShadowViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", ShadowProjectionMatrixFinite());
    _shadowBuffer->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->RenderDepth(Render::Mode::All);
    _shadowBuffer->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
}

glm::mat4 DirectionnalLight::ShadowProjectionMatrixInfinite() const
{
    auto viewMatrix = ShadowViewMatrix();

    std::array<glm::vec3, 8> vertex = ExtractFrustum(Scene::Current()->CurrentCamera());
    glm::vec3 maxOrtho = viewMatrix * glm::vec4(vertex.at(0), 1);
    glm::vec3 minOrtho = viewMatrix * glm::vec4(vertex.at(0), 1);
    for (auto& v : vertex) {
        v = viewMatrix * glm::vec4(v, 1);
        maxOrtho.x = std::max(maxOrtho.x, v.x);
        maxOrtho.y = std::max(maxOrtho.y, v.y);
        maxOrtho.z = std::max(maxOrtho.z, v.z);
        minOrtho.x = std::min(minOrtho.x, v.x);
        minOrtho.y = std::min(minOrtho.y, v.y);
        minOrtho.z = std::min(minOrtho.z, v.z);
    }
    auto limits = glm::vec4(minOrtho.x, maxOrtho.x, minOrtho.y, maxOrtho.y);
    auto zfar = -maxOrtho.z;
    auto znear = -minOrtho.z;
    return glm::ortho(limits.x, limits.y, limits.z, limits.w, znear, zfar);
}

glm::mat4 DirectionnalLight::ShadowProjectionMatrixFinite() const
{
    auto viewMatrix = ShadowViewMatrix();

    static std::array<glm::vec3, 7> vertex {};
    vertex.at(0) = Max();
    vertex.at(1) = glm::vec3(Min().x, Min().y, Max().z);
    vertex.at(2) = glm::vec3(Min().x, Max().y, Max().z);
    vertex.at(3) = glm::vec3(Min().x, Max().y, Min().z);
    vertex.at(4) = glm::vec3(Max().x, Max().y, Min().z);
    vertex.at(5) = glm::vec3(Max().x, Min().y, Min().z);
    vertex.at(6) = glm::vec3(Max().x, Min().y, Max().z);
    glm::vec3 maxOrtho = viewMatrix * glm::vec4(Min(), 1);
    glm::vec3 minOrtho = viewMatrix * glm::vec4(Min(), 1);
    for (auto& v : vertex) {
        v = viewMatrix * glm::vec4(v, 1);
        maxOrtho.x = std::max(maxOrtho.x, v.x);
        maxOrtho.y = std::max(maxOrtho.y, v.y);
        maxOrtho.z = std::max(maxOrtho.z, v.z);
        minOrtho.x = std::min(minOrtho.x, v.x);
        minOrtho.y = std::min(minOrtho.y, v.y);
        minOrtho.z = std::min(minOrtho.z, v.z);
    }
    auto limits = glm::vec4(minOrtho.x, maxOrtho.x, minOrtho.y, maxOrtho.y);
    auto zfar = -maxOrtho.z;
    auto znear = -minOrtho.z;
    return glm::ortho(limits.x, limits.y, limits.z, limits.w, znear, zfar);
}

glm::mat4 DirectionnalLight::ShadowViewMatrix() const
{
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - GetDirection() * radius;
    return glm::lookAt(camPos, WorldPosition(), GetUp(GetDirection()));
}

std::shared_ptr<Shader::Program> DirectionnalLight::_GetShader() const
{
    return _shader;
}

void DirectionnalLight::_SetShader(std::shared_ptr<Shader::Program> shader)
{
    _shader = shader;
}

std::shared_ptr<Framebuffer> DirectionnalLight::_GetShadowBuffer() const
{
    return _shadowBuffer;
}

void DirectionnalLight::_SetShadowBuffer(std::shared_ptr<Framebuffer> shadowBuffer)
{
    _shadowBuffer = shadowBuffer;
}
