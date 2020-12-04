/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-11-27 22:54:08
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
#include "Shader/Shader.hpp"

#include <glm/vec3.hpp>

auto DirectionnalLightGeometry()
{
    static auto geometry = CubeMesh::CreateGeometry("DirectionnalLightGeometry", glm::vec3(1));
    return geometry;
}

auto DirectionnalLightShader()
{
    static std::shared_ptr<Shader> shader;
    if (shader == nullptr) {
        auto lightingFragmentCode =
#include "directionnalLight.frag"
            ;
        auto lightingVertexCode =
#include "light.vert"
            ;
        shader = Component::Create<Shader>("DirectionnalLightShader", Shader::Type::LightingShader);
        shader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(lightingVertexCode, "TransformGeometry();")));
        shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(lightingFragmentCode, "Lighting();"));
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
        SetComponent(Component::Create<Framebuffer>(Name() + "_shadowMap", glm::vec2(Config::Get("ShadowRes", 1024)), 0, 0));
        GetComponent<Framebuffer>()->Create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24);
        GetComponent<Framebuffer>()->depth()->set_parameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        GetComponent<Framebuffer>()->depth()->set_parameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
}

glm::vec3 DirectionnalLight::Direction() const
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
    std::array<glm::vec3, 8> NDCCube{
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
    auto shader = GetComponent<Shader>();
    auto geometryBuffer = Render::GeometryBuffer();
    glm::vec3 geometryPosition;
    if (Infinite())
        geometryPosition = Scene::Current()->CurrentCamera()->WorldPosition();
    else
        geometryPosition = Parent() ? Parent()->WorldPosition() + GetPosition() : GetPosition();
    if (GetCastShadow()) {
        shader->SetDefine("SHADOW");
        shader->SetTexture("Light.Shadow", GetComponent<Framebuffer>()->depth());
    }
    else
        shader->RemoveDefine("SHADOW");
    shader->SetUniform("Light.Max", Max());
    shader->SetUniform("Light.Min", Min());
    shader->SetUniform("Light.Projection", Infinite() ? ShadowProjectionMatrixInfinite() : ShadowProjectionMatrixFinite());
    shader->SetUniform("Light.Color", GetColor());
    shader->SetUniform("Light.Direction", Direction());
    shader->SetUniform("Light.Infinite", Infinite());
    shader->SetUniform("Matrix.Model", glm::translate(geometryPosition) * LocalScaleMatrix());
    shader->SetTexture("Texture.Geometry.F0", geometryBuffer->attachement(2));
    shader->SetTexture("Texture.Geometry.Normal", geometryBuffer->attachement(4));
    shader->SetTexture("Texture.Geometry.Depth", geometryBuffer->depth());
    GetComponent<Shader>()->use();
    DirectionnalLightGeometry()->Draw();
    GetComponent<Shader>()->use(false);
}

void DirectionnalLight::DrawShadowInfinite()
{
    auto camera = Scene::Current()->CurrentCamera();
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - Direction() * radius;
    auto viewMatrix = glm::lookAt(camPos, WorldPosition(), GetUp(Direction()));

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
    static auto tempCamera = Component::Create<Camera>("light_camera", Camera::Projection::Ortho);
    tempCamera->SetZnear(znear);
    tempCamera->SetZfar(zfar);
    tempCamera->SetFrustum(limits);
    tempCamera->SetPosition(camPos);
    tempCamera->LookAt(WorldPosition(), GetUp(Direction()));
    Scene::Current()->SetCurrentCamera(tempCamera);
    GetComponent<Framebuffer>()->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->RenderDepth(RenderMod::RenderAll);
    GetComponent<Framebuffer>()->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
}

void DirectionnalLight::DrawShadowFinite()
{
    auto camera = Scene::Current()->CurrentCamera();
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - Direction() * radius;
    auto viewMatrix = glm::lookAt(camPos, WorldPosition(), GetUp(Direction()));

    static std::array<glm::vec3, 7> vertex{};
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
    static auto tempCamera = Component::Create<Camera>("light_camera", Camera::Projection::Ortho);
    tempCamera->SetZnear(znear);
    tempCamera->SetZfar(zfar);
    tempCamera->SetFrustum(limits);
    tempCamera->SetPosition(camPos);
    tempCamera->LookAt(WorldPosition(), GetUp(Direction()));
    Scene::Current()->SetCurrentCamera(tempCamera);
    GetComponent<Framebuffer>()->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->RenderDepth(RenderMod::RenderAll);
    GetComponent<Framebuffer>()->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
}

#include "Camera/Camera.hpp"

glm::mat4 DirectionnalLight::ShadowProjectionMatrixInfinite() const
{
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - Direction() * radius;
    auto viewMatrix = glm::lookAt(camPos, WorldPosition(), GetUp(Direction()));

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
    return glm::ortho(limits.x, limits.y, limits.z, limits.w, znear, zfar) * viewMatrix;
}

glm::mat4 DirectionnalLight::ShadowProjectionMatrixFinite() const
{
    auto radius = glm::distance(WorldPosition(), Max());
    auto camPos = WorldPosition() - Direction() * radius;
    auto viewMatrix = glm::lookAt(camPos, WorldPosition(), GetUp(Direction()));

    static std::array<glm::vec3, 7> vertex{};
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
    return glm::ortho(limits.x, limits.y, limits.z, limits.w, znear, zfar) * viewMatrix;
}

//void DirectionnalLight::UpdateTransformMatrix()
//{
//    SetTransformMatrix(glm::lookAt(Position(), glm::vec3(0, 0, 0), Common::Up()));
//}

