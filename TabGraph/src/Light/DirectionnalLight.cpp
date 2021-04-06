/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-15 00:04:45
*/

#include "Light/DirectionnalLight.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Config.hpp"
#include "Framebuffer.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/SceneRenderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Global.hpp"
#include "Shader/Program.hpp"
#include "Texture/Cubemap.hpp"
#include "SphericalHarmonics.hpp"

#include <glm/glm.hpp>

static inline auto DirectionnalLightGeometry()
{
    static auto geometry = CubeMesh::CreateGeometry("DirectionnalLightGeometry", glm::vec3(1));
    return geometry;
}

static inline auto DirectionnalLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DirectionnalLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto randomCode =
#include "Random.glsl"
        ;
    auto shadowCode =
#include "SampleShadowMap.glsl"
        ;
    auto deferredDirLightFragmentCode =
#include "Lights/DeferredDirectionnalLight.frag"
        ;
    Shader::Stage::Code shaderCode =
        Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code { randomCode } +
        Shader::Stage::Code { shadowCode } +
        Shader::Stage::Code { deferredDirLightFragmentCode, "Lighting();" };
    return shaderCode;
}

static inline auto DeferredDirectionnalLightShader()
{
    std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        shader = Component::Create<Shader::Program>("DirectionnalLightShader");
        shader->SetDefine("Pass", "DeferredLighting");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DirectionnalLightVertexCode()));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DirectionnalLightFragmentCode()));
    }
    return shader;
}

static inline auto ProbeDirectionnalLightShader()
{
    std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        auto LayeredCubemapRenderCode =
#include "LayeredCubemapRender.geom"
            ;
        auto deferredVertexCode =
#include "deferred.vert"
            ;
        auto dirLightFragCode =
#include "Lights/ProbeDirectionnalLight.frag"
            ;
        shader = Component::Create<Shader::Program>("DirectionnalLUTShader");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Geometry, { LayeredCubemapRenderCode, "LayeredCubemapRender();" }));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { dirLightFragCode, "Lighting();" }));
    }
    return shader;
}

DirectionnalLight::DirectionnalLight()
{
    _probeShader = ProbeDirectionnalLightShader();
    _deferredShader = DeferredDirectionnalLightShader();
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
    SetCastShadow(cast_shadow);
    if (cast_shadow) {
        auto shadowRes{ glm::vec2(Config::Global().Get("ShadowRes", 1024)) };
        _SetShadowBuffer(Component::Create<Framebuffer>(GetName() + "_shadowMap", shadowRes, 0, 0));
        _GetShadowBuffer()->SetDepthBuffer(Component::Create<Texture2D>(shadowRes, Pixel::SizedFormat::Depth24));
        _GetShadowBuffer()->GetDepthBuffer()->SetParameter<Texture::Parameter::CompareMode>(Texture::CompareMode::CompareRefToTexture);
        _GetShadowBuffer()->GetDepthBuffer()->SetParameter<Texture::Parameter::CompareFunc>(Texture::CompareFunc::LessEqual);
    }
}

void DirectionnalLight::SetDirection(const glm::vec3& direction)
{
    _SetDirection(normalize(direction));
}

glm::vec3 DirectionnalLight::GetHalfSize() const
{
    return GetScale() / 2.f;
}

void DirectionnalLight::SetHalfSize(const glm::vec3& halfSize)
{
    SetScale(halfSize * 2.f);
}

glm::vec3 DirectionnalLight::GetMin() const
{
    return WorldPosition() - GetHalfSize();
}

glm::vec3 DirectionnalLight::GetMax() const
{
    return WorldPosition() + GetHalfSize();
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
    auto invVP = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
    for (auto& v : NDCCube) {
        glm::vec4 normalizedCoord = invVP * glm::vec4(v, 1);
        v = glm::vec3(normalizedCoord) / normalizedCoord.w;
    }
    return NDCCube;
}

void DirectionnalLight::render_shadow()
{
    if (GetCastShadow())
        GetInfinite() ? DrawShadowInfinite() : DrawShadowFinite();
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
    auto geometryBuffer = Renderer::DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (GetInfinite())
        geometryPosition = Scene::Current()->CurrentCamera()->WorldPosition();
    else
        geometryPosition = GetParent() ? GetParent()->WorldPosition() + GetPosition() : GetPosition();
    if (GetCastShadow())
        _deferredShader->SetDefine("SHADOW");
    else
        _deferredShader->RemoveDefine("SHADOW");
    glDisable(GL_CULL_FACE);
    _deferredShader->Use()
        .SetUniform("Light.Color", GetColor())
        .SetUniform("Light.Direction", GetDirection())
        .SetTexture("Light.Shadow", GetCastShadow() ? _GetShadowBuffer()->GetDepthBuffer() : nullptr)
        .SetUniform("Light.Max", GetMax())
        .SetUniform("Light.Min", GetMin())
        .SetUniform("Light.Projection", (GetInfinite() ? ShadowProjectionMatrixInfinite() : ShadowProjectionMatrixFinite()) * ShadowViewMatrix())
        .SetUniform("Light.Infinite", GetInfinite()).SetUniform("GeometryMatrix", glm::translate(geometryPosition) * GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(DirectionnalLightGeometry(), true);
    _deferredShader->Done();
}

void DirectionnalLight::DrawShadowInfinite()
{
    auto camera = Scene::Current()->CurrentCamera();
    auto radius = glm::distance(WorldPosition(), GetMax());
    auto camPos = WorldPosition() - GetDirection() * radius;
    static std::shared_ptr<Camera> tempCamera(new Camera("dirLightCamera"));
    tempCamera->SetProjectionMatrix(ShadowProjectionMatrixInfinite());
    tempCamera->SetViewMatrix(ShadowViewMatrix());

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", ShadowViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", ShadowProjectionMatrixInfinite());
    _shadowBuffer->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::Render(Scene::Current(), {
        Renderer::Options::Pass::ShadowDepth,
        Renderer::Options::Mode::All,
        tempCamera,
        Scene::Current(),
        Renderer::FrameNumber()
    });
    _shadowBuffer->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->GetProjectionMatrix());
}

void DirectionnalLight::DrawShadowFinite()
{
    auto camera = Scene::Current()->CurrentCamera();
    auto radius = glm::distance(WorldPosition(), GetMax());
    auto camPos = WorldPosition() - GetDirection() * radius;
    static std::shared_ptr<Camera> tempCamera(new Camera("dirLightCamera"));
    tempCamera->SetProjectionMatrix(ShadowProjectionMatrixFinite());
    tempCamera->SetViewMatrix(ShadowViewMatrix());

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", ShadowViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", ShadowProjectionMatrixFinite());
    _shadowBuffer->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::Render(Scene::Current(), {
        Renderer::Options::Pass::ShadowDepth,
        Renderer::Options::Mode::All,
        tempCamera,
        Scene::Current(),
        Renderer::FrameNumber()
    });
    _shadowBuffer->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->GetProjectionMatrix());
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
    const auto min { GetMin() }, max { GetMax() };
    vertex.at(0) = GetMax();
    vertex.at(1) = glm::vec3(min.x, min.y, max.z);
    vertex.at(2) = glm::vec3(min.x, max.y, max.z);
    vertex.at(3) = glm::vec3(min.x, max.y, min.z);
    vertex.at(4) = glm::vec3(max.x, max.y, min.z);
    vertex.at(5) = glm::vec3(max.x, min.y, min.z);
    vertex.at(6) = glm::vec3(max.x, min.y, max.z);
    glm::vec3 maxOrtho = viewMatrix * glm::vec4(min, 1);
    glm::vec3 minOrtho = viewMatrix * glm::vec4(min, 1);
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
    auto radius = glm::distance(WorldPosition(), GetMax());
    auto camPos = WorldPosition() - GetDirection() * radius;
    return glm::lookAt(camPos, WorldPosition(), GetUp(GetDirection()));
}

std::shared_ptr<Framebuffer> DirectionnalLight::_GetShadowBuffer() const
{
    return _shadowBuffer;
}

void DirectionnalLight::_SetShadowBuffer(std::shared_ptr<Framebuffer> shadowBuffer)
{
    _shadowBuffer = shadowBuffer;
}

#include "Light/LightProbe.hpp"

void DirectionnalLight::DrawProbe(LightProbe& lightProbe)
{
    static auto diffuseSH{
        lightProbe.GetSphericalHarmonics().ProjectFunction(
            [=](const SphericalHarmonics::Sample& sample) {
                return GetColor() * glm::dot(sample.vec, glm::normalize(GetDirection()));
            })
    };
    for (auto i = 0u; i < diffuseSH.size(); ++i)
        lightProbe.GetDiffuseSH().at(i) += diffuseSH.at(i);
    lightProbe.GetReflectionBuffer()->bind();
    _probeShader->Use()
        .SetUniform("SpecularMode", true)
        .SetUniform("Light.Color", GetColor())
        .SetUniform("Light.Direction", GetDirection());
    Renderer::Render(Renderer::DisplayQuad());
    //Framebuffer::bind_default();
}
