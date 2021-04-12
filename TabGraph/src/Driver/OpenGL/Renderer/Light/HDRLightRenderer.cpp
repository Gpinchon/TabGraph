/*
* @Author: gpinchon
* @Date:   2021-04-11 14:44:44
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 14:44:54
*/

#include "Driver/OpenGL/Renderer/Light/HDRLightRenderer.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Camera/Camera.hpp"
#include "Framebuffer.hpp"
#include "Light/HDRLight.hpp"
#include "Light/LightProbe.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Shader/Program.hpp"
#include "Shader/Stage.hpp"
#include "SphericalHarmonics.hpp"
#include "Texture/Cubemap.hpp"

const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
static inline glm::vec2 SampleSphericalMap(glm::vec3 xyz)
{
    glm::vec2 uv = glm::vec2(atan2(xyz.z, xyz.x), asin(xyz.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

static inline auto HDRLightGeometry()
{
    static auto geometry = CubeMesh::CreateGeometry("HDRLightGeometry", glm::vec3(1));
    return geometry;
}

static inline auto DeferredHDRLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    Shader::Stage::Code shaderCode{ lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto DeferredHDRLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto randomCode =
#include "Random.glsl"
        ;
    auto SHCode =
#include "sphericalHarmonics.glsl"
        ;
    auto lightingFragmentCode =
#include "Lights/DeferredHDRLight.frag"
        ;
    Shader::Stage::Code shaderCode = Shader::Stage::Code{ deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code{ SHCode } + Shader::Stage::Code{ lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

namespace Renderer {
static SphericalHarmonics s_SH{ 50 };
HDRLightRenderer::Impl::Impl()
{
    auto LayeredCubemapRenderCode =
#include "LayeredCubemapRender.geom"
        ;
    auto deferredVertexCode =
#include "deferred.vert"
        ;
    auto dirLightFragCode =
#include "Lights/ProbeHDRLight.frag"
        ;
    _probeShader = Component::Create<Shader::Program>("ProbeHDRLightShader");
    _probeShader->Attach(Shader::Stage(Shader::Stage::Type::Geometry, { LayeredCubemapRenderCode, "LayeredCubemapRender();" }));
    _probeShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertexCode, "FillVertexData();" }));
    _probeShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { dirLightFragCode, "Lighting();" }));
    _deferredShader = Component::Create<Shader::Program>("DeferredHDRLightShader");
    _deferredShader->SetDefine("Pass", "DeferredLighting");
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredHDRLightVertexCode()));
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DeferredHDRLightFragmentCode()));
}
void HDRLightRenderer::Impl::FlagDirty()
{
	_dirty = true;
}

void HDRLightRenderer::Impl::Render(Light& light, const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::DeferredLighting) {
        _RenderDeferredLighting(static_cast<HDRLight&>(light), options);
    }
}

void HDRLightRenderer::Impl::UpdateLightProbe(Light& light, LightProbe& lightProbe)
{
    auto& hdrLight{ static_cast<HDRLight&>(light) };
    _Update(hdrLight);
    for (auto i = 0u; i < std::min(_SHDiffuse.size(), lightProbe.GetDiffuseSH().size()); ++i)
        lightProbe.GetDiffuseSH().at(i) += _SHDiffuse.at(i);
    lightProbe.GetReflectionBuffer()->bind();
    _probeShader->Use()
        .SetUniform("Light.Min", hdrLight.GetMin())
        .SetUniform("Light.Max", hdrLight.GetMax())
        .SetUniform("Light.Infinite", hdrLight.GetInfinite())
        .SetUniform("ProbePosition", lightProbe.GetAbsolutePosition())
        .SetTexture("ReflectionMap", hdrLight.GetReflection());
    Renderer::Render(Renderer::DisplayQuad());
}

void HDRLightRenderer::Impl::_RenderDeferredLighting(HDRLight& light, const Renderer::Options& options)
{
    _Update(light);
    auto geometryBuffer = Renderer::DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (light.GetInfinite())
        geometryPosition = options.camera->WorldPosition();
    else
        geometryPosition = light.GetParent() ? light.GetParent()->WorldPosition() + light.GetPosition() : light.GetPosition();
    _deferredShader->Use()
        .SetTexture("ReflectionMap", light.GetReflection())
        .SetTexture("DefaultBRDFLUT", Renderer::DefaultBRDFLUT())
        .SetUniform("SH[0]", _SHDiffuse.data(), _SHDiffuse.size())
        .SetUniform("Light.Max", light.GetMax())
        .SetUniform("Light.Min", light.GetMin())
        .SetUniform("Light.Infinite", light.GetInfinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * light.GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(HDRLightGeometry(), true);
    _deferredShader->Done();
}

void HDRLightRenderer::Impl::_Update(HDRLight& light)
{
    if (!_dirty)
        return;
    auto asset{ light.GetComponent<Asset>() };
    asset->Load();
    assert(asset->GetAssetType() == Image::AssetType);
    auto image{ asset->GetComponent<Image>() };
    _SHDiffuse = s_SH.ProjectFunction(
        [&](const SphericalHarmonics::Sample& sample) {
            auto dir{ -sample.vec };
            auto uv{ SampleSphericalMap(dir) };
            glm::ivec2 texCoord{
                uv.x * image->GetSize().x,
                uv.y * image->GetSize().y
            };
            texCoord = glm::clamp(texCoord, glm::ivec2(0), image->GetSize() - 1);
            return image->GetColor(texCoord);
        }
    );
    light.RemoveComponent(asset);
    _dirty = false;
}
};