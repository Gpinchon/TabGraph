/*
* @Author: gpinchon
* @Date:   2021-04-10 15:35:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-10 16:21:36
*/

#include <Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.hpp>
#include <Driver/OpenGL/Texture/Framebuffer.hpp>
#include <Camera/Camera.hpp>
#include <Config.hpp>
#include <Light/DirectionalLight.hpp>
#include <Light/LightProbe.hpp>
#include <Surface/CubeMesh.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Surface/GeometryRenderer.hpp>
#include <Renderer/SceneRenderer.hpp>
#include <Shader/Global.hpp>
#include <Shader/Program.hpp>
#include <SphericalHarmonics.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/TextureSampler.hpp>
#include <Window.hpp>

#include <GL/glew.h>
#include <array>

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
    Shader::Stage::Code shaderCode{ lightingVertexCode, "TransformGeometry();" };
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
        Shader::Stage::Code{ deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code{ randomCode } +
        Shader::Stage::Code{ shadowCode } +
        Shader::Stage::Code{ deferredDirLightFragmentCode, "Lighting();" };
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

namespace Renderer {
DirectionalLightRenderer::DirectionalLightRenderer(DirectionalLight& light)
    : LightRenderer(light)
{
    _probeShader = ProbeDirectionnalLightShader();
    _deferredShader = DeferredDirectionnalLightShader();
}
void DirectionalLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::ShadowDepth)
        _RenderShadow(static_cast<DirectionalLight&>(_light), options);
    else if (options.pass == Renderer::Options::Pass::DeferredLighting)
        _RenderDeferredLighting(static_cast<DirectionalLight&>(_light), options);
}

void DirectionalLightRenderer::UpdateLightProbe(const Renderer::Options& options, LightProbe& lightProbe)
{
    auto &dirLight{ static_cast<DirectionalLight&>(_light) };
    //TODO implement FlagDirty mechanism
    static auto diffuseSH{
        lightProbe.GetSphericalHarmonics().ProjectFunction(
            [&](const SphericalHarmonics::Sample& sample) {
                return dirLight.GetColor() * glm::dot(sample.vec, glm::normalize(dirLight.GetDirection()));
            })
    };
    for (auto i = 0u; i < std::min(diffuseSH.size(), lightProbe.GetDiffuseSH().size()); ++i)
        lightProbe.GetDiffuseSH().at(i) += diffuseSH.at(i);
    OpenGL::Framebuffer::Bind(lightProbe.GetReflectionBuffer());
    options.renderer->SetViewPort(lightProbe.GetReflectionBuffer()->GetSize());
    _probeShader->Use()
        .SetUniform("SpecularMode", true)
        .SetUniform("Light.SpecularFactor", dirLight.GetSpecularFactor())
        .SetUniform("Light.DiffuseFactor", dirLight.GetDiffuseFactor())
        .SetUniform("Light.Color", dirLight.GetColor())
        .SetUniform("Light.Direction", dirLight.GetDirection());
    Renderer::Render(options.renderer->GetDisplayQuad(), true);
    OpenGL::Framebuffer::Bind(nullptr);
}

void DirectionalLightRenderer::_RenderDeferredLighting(DirectionalLight& light, const Renderer::Options& options)
{
    auto geometryBuffer = options.renderer->DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (light.GetInfinite())
        geometryPosition = options.camera->WorldPosition();
    else
        geometryPosition = light.GetParent() ? light.GetParent()->WorldPosition() + light.GetPosition() : light.GetPosition();
    if (light.GetCastShadow()) {
        _deferredShader->SetDefine("SHADOWBLURRADIUS", std::to_string(light.GetShadowBlurRadius()));
        _deferredShader->SetDefine("SHADOW");
    }
    else
        _deferredShader->RemoveDefine("SHADOW");
    glDisable(GL_CULL_FACE);
    
    _deferredShader->Use()
        .SetUniform("Light.DiffuseFactor", light.GetDiffuseFactor())
        .SetUniform("Light.SpecularFactor", light.GetSpecularFactor())
        .SetUniform("Light.Color", light.GetColor())
        .SetUniform("Light.Direction", light.GetDirection())
        .SetTexture("Light.Shadow", light.GetCastShadow() ? _shadowBuffer->GetDepthBuffer() : nullptr)
        .SetUniform("Light.Max", light.GetMax())
        .SetUniform("Light.Min", light.GetMin())
        .SetUniform("Light.Projection", (light.GetInfinite() ?
            DirectionalLightShadowProjectionMatrixInfinite(light, options) :
            DirectionalLightShadowProjectionMatrixFinite(light)) * DirectionalLightShadowViewMatrix(light))
        .SetUniform("Light.Infinite", light.GetInfinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * light.GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    glCullFace(GL_FRONT);
    Renderer::Render(DirectionnalLightGeometry(), false);
    glCullFace(GL_BACK);
    _deferredShader->Done();
}

void DirectionalLightRenderer::_RenderShadow(DirectionalLight& light, const Renderer::Options& options)
{
    if (light.GetCastShadow()) {
        auto shadowRes{ glm::ivec2(light.GetShadowResolution()) };
        if (_shadowBuffer == nullptr) {
            auto shadowBuffer{ Component::Create<Texture2D>(shadowRes, Pixel::SizedFormat::Depth24) };
            shadowBuffer->GetTextureSampler()->SetCompareMode(TextureSampler::CompareMode::CompareRefToTexture);
            shadowBuffer->GetTextureSampler()->SetCompareFunc(TextureSampler::CompareFunc::LessEqual);
            shadowBuffer->SetMipMapNbr(1);
            _shadowBuffer = std::make_shared<Framebuffer>(shadowRes);
            _shadowBuffer->SetDepthBuffer(shadowBuffer);
        }
        _shadowBuffer->SetSize(shadowRes);
        light.GetInfinite() ? _RenderShadowInfinite(light, options) : _RenderShadowFinite(light, options);
    }
}

void DirectionalLightRenderer::_RenderShadowInfinite(DirectionalLight& light, const Renderer::Options& options)
{
    auto radius = glm::distance(light.WorldPosition(), light.GetMax());
    auto camPos = light.WorldPosition() - light.GetDirection() * radius;
    static std::shared_ptr<Camera> tempCamera(new Camera("dirLightCamera"));
    tempCamera->SetProjectionMatrix(DirectionalLightShadowProjectionMatrixInfinite(light, options));
    tempCamera->SetViewMatrix(DirectionalLightShadowViewMatrix(light));

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", DirectionalLightShadowViewMatrix(light));
    Shader::Global::SetUniform("Camera.Matrix.Projection", DirectionalLightShadowProjectionMatrixInfinite(light, options));
    OpenGL::Framebuffer::Bind(_shadowBuffer);
    options.renderer->SetViewPort(_shadowBuffer->GetSize());
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::Render(options.scene, {
        Renderer::Options::Pass::ShadowDepth,
        Renderer::Options::Mode::All,
        tempCamera,
        options.scene,
        options.renderer,
        options.frameNumber,
        options.delta
        });
    OpenGL::Framebuffer::Bind(nullptr);
    //options.scene->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", options.camera->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", options.camera->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", options.camera->GetProjectionMatrix(options.renderer->GetWindow()->GetSize()));
}

void DirectionalLightRenderer::_RenderShadowFinite(DirectionalLight& light, const Renderer::Options& options)
{
    auto radius = glm::distance(light.WorldPosition(), light.GetMax());
    auto camPos = light.WorldPosition() - light.GetDirection() * radius;
    static std::shared_ptr<Camera> tempCamera(new Camera("dirLightCamera"));
    tempCamera->SetProjectionMatrix(DirectionalLightShadowProjectionMatrixFinite(light));
    tempCamera->SetViewMatrix(DirectionalLightShadowViewMatrix(light));

    Shader::Global::SetUniform("Camera.Position", camPos);
    Shader::Global::SetUniform("Camera.Matrix.View", DirectionalLightShadowViewMatrix(light));
    Shader::Global::SetUniform("Camera.Matrix.Projection", DirectionalLightShadowProjectionMatrixFinite(light));
    OpenGL::Framebuffer::Bind(_shadowBuffer);
    options.renderer->SetViewPort(_shadowBuffer->GetSize());
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::Render(options.scene, {
        Renderer::Options::Pass::ShadowDepth,
        Renderer::Options::Mode::All,
        tempCamera,
        options.scene,
        options.renderer,
        options.frameNumber,
        options.delta
        });
    OpenGL::Framebuffer::Bind(nullptr);
    //options.scene->SetCurrentCamera(camera);
    Shader::Global::SetUniform("Camera.Position", options.camera->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", options.camera->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", options.camera->GetProjectionMatrix(options.renderer->GetWindow()->GetSize()));
}
};

static inline auto GetUp(const glm::vec3& direction)
{
    auto up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(normalize(direction), up)) > .9999f)
        up = glm::vec3(1, 0, 0);
    return up;
}

glm::mat4 DirectionalLightShadowViewMatrix(DirectionalLight& light)
{
    auto radius = glm::distance(light.WorldPosition(), light.GetMax());
    auto camPos = light.WorldPosition() - light.GetDirection() * radius;
    return glm::lookAt(camPos, light.WorldPosition(), GetUp(light.GetDirection()));
}

glm::mat4 DirectionalLightShadowProjectionMatrixInfinite(DirectionalLight& light, const Renderer::Options& options)
{
    auto viewMatrix = DirectionalLightShadowViewMatrix(light);

    auto vertex = options.camera->ExtractFrustum(options.renderer->GetWindow()->GetSize());
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

glm::mat4 DirectionalLightShadowProjectionMatrixFinite(DirectionalLight& light)
{
    auto viewMatrix = DirectionalLightShadowViewMatrix(light);

    static std::array<glm::vec3, 7> vertex{};
    const auto min{ light.GetMin() }, max{ light.GetMax() };
    vertex.at(0) = max;
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