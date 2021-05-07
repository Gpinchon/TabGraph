/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:26
*/

#include "Driver/OpenGL/Renderer/Light/SkyLightRenderer.hpp"
#include "Camera/Camera.hpp"
#include "Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.hpp"
#include "Framebuffer.hpp"
#include "Light/SkyLight.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include "Shader/Program.hpp"
#include "SphericalHarmonics.hpp"
#include "Texture/Texture2D.hpp"
#include "Texture/TextureCubemap.hpp"
#include "Texture/TextureSampler.hpp"

static inline auto DeferredSkyLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    static Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DeferredSkyLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto SHCode =
#include "sphericalHarmonics.glsl"
        ;
    auto lightingFragmentCode =
#include "Lights/DeferredSkyLight.frag"
        ;
    Shader::Stage::Code shaderCode = Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { SHCode } + Shader::Stage::Code { lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

auto SkyLightLUTShader()
{
    static std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        auto LayeredCubemapRenderCode =
#include "LayeredCubemapRender.geom"
            ;
        auto deferredVertexCode =
#include "deferred.vert"
            ;
        auto skyLightFragCode =
#include "Lights/ProbeSkyLight.frag"
            ;
        shader = Component::Create<Shader::Program>("DirectionnalLUTShader");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Geometry, { LayeredCubemapRenderCode, "LayeredCubemapRender();" }));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { skyLightFragCode, "ComputeSkyLight();" }));
    }
    return shader;
}

static inline auto SkyLightGeometry()
{
    static auto geometry = CubeMesh::CreateGeometry("SkyLightGeometry", glm::vec3(1));
    return geometry;
}

namespace Renderer {
static SphericalHarmonics s_SH { 50 };
SkyLightRenderer::SkyLightRenderer(SkyLight& light)
    : LightRenderer(light)
{
    _deferredShader = Component::Create<Shader::Program>("SkyLightShader");
    _deferredShader->SetDefine("Pass", "DeferredLighting");
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredSkyLightVertexCode()));
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DeferredSkyLightFragmentCode()));
}

void SkyLightRenderer::FlagDirty()
{
    _dirty = true;
}

void SkyLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::DeferredLighting)
        _RenderDeferredLighting(static_cast<SkyLight&>(_light), options);
    else if (options.pass == Renderer::Options::Pass::ShadowDepth)
        _RenderDeferredLighting(static_cast<SkyLight&>(_light), options);
}

void SkyLightRenderer::UpdateLightProbe(LightProbe& lightProbe)
{
    _UpdateLUT(static_cast<SkyLight&>(_light));
    //TODO really implement this
}

void SkyLightRenderer::_RenderDeferredLighting(SkyLight& light, const Renderer::Options& options)
{
    if (_dirty) {
        _UpdateLUT(light);
        //re-bind lighting buffer
        Renderer::DeferredLightingBuffer()->bind();
    }
    auto geometryBuffer = Renderer::DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (light.GetInfinite())
        geometryPosition = options.camera->WorldPosition();
    else
        geometryPosition = light.GetParent() ? light.GetParent()->WorldPosition() + light.GetPosition() : light.GetPosition();
    if (light.GetCastShadow())
        _deferredShader->SetDefine("SHADOW");
    else
        _deferredShader->RemoveDefine("SHADOW");
    _deferredShader->Use()
        .SetTexture("SpecularLUT", _reflectionLUT)
        .SetTexture("DefaultBRDFLUT", Renderer::DefaultBRDFLUT())
        .SetUniform("SH[0]", _SHDiffuse.data(), _SHDiffuse.size())
        .SetTexture("Light.Shadow", light.GetCastShadow() ? _shadowBuffer->GetDepthBuffer() : nullptr)
        .SetUniform("Light.SpecularPower", light.GetSpecularPower())
        .SetUniform("Light.Max", light.GetMax())
        .SetUniform("Light.Min", light.GetMin())
        .SetUniform("Light.Projection", (light.GetInfinite() ? DirectionalLightShadowProjectionMatrixInfinite(light, options) : DirectionalLightShadowProjectionMatrixFinite(light)) * DirectionalLightShadowViewMatrix(light))
        .SetUniform("Light.Infinite", light.GetInfinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * light.GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(SkyLightGeometry(), true);
    _deferredShader->Done();
}
void SkyLightRenderer::_UpdateLUT(SkyLight& light)
{
    if (!_dirty)
        return;
    _SHDiffuse = s_SH.ProjectFunction(
        [&](const SphericalHarmonics::Sample& sample) {
            return min(light.GetIncidentLight(sample.vec), 1.f);
        });
    if (_reflectionLUT == nullptr) {
        _reflectionLUT = Component::Create<TextureCubemap>(glm::ivec2(256), Pixel::SizedFormat::Float16_RGB);
    }
    static auto s_diffuseLUTBuffer = Component::Create<Framebuffer>("DiffuseLUTBuffer", glm::ivec2(256));
    s_diffuseLUTBuffer->SetColorBuffer(_reflectionLUT, 0);
    s_diffuseLUTBuffer->bind();
    SkyLightLUTShader()->Use().SetUniform("BetaMie", light.GetBetaMie()).SetUniform("BetaRayleigh", light.GetBetaRayleigh()).SetUniform("SunDirection", light.GetSunDirection()).SetUniform("SunPower", light.GetSunPower()).SetUniform("PlanetRadius", light.GetPlanetRadius()).SetUniform("AtmosphereRadius", light.GetAtmosphereRadius()).SetUniform("HRayLeigh", light.GetHRayleigh()).SetUniform("HMie", light.GetHMie());
    glClear(GL_COLOR_BUFFER_BIT);
    Renderer::Render(Renderer::DisplayQuad(), true);
    Framebuffer::bind_default();
    s_diffuseLUTBuffer->SetColorBuffer(nullptr, 0);
    _reflectionLUT->GenerateMipmap();
    _reflectionLUT->GetTextureSampler()->SetMinFilter(TextureSampler::Filter::LinearMipmapLinear);
    _dirty = false;
}
};