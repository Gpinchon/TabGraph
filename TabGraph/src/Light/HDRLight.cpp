/*
* @Author: gpinchon
* @Date:   2021-03-14 23:55:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-18 23:28:30
*/

#include "Light/HDRLight.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Camera/Camera.hpp"
#include "Framebuffer.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Program.hpp"
#include "Shader/Stage.hpp"
#include "Texture/Cubemap.hpp"
#include "Light/LightProbe.hpp"
#include "SphericalHarmonics.hpp"

static SphericalHarmonics s_SH { 50 };

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
    Shader::Stage::Code shaderCode { lightingVertexCode, "TransformGeometry();" };
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
    Shader::Stage::Code shaderCode = Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { SHCode } + Shader::Stage::Code { lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

HDRLight::HDRLight(std::shared_ptr<Asset> hdrTexture)
{
    SetHDRTexture(hdrTexture);
    _SetReflection(Component::Create<Cubemap>(hdrTexture));
    _deferredShader = Component::Create<Shader::Program>("DeferredHDRLightShader");
    _deferredShader->SetDefine("Pass", "DeferredLighting");
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredHDRLightVertexCode()));
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DeferredHDRLightFragmentCode()));

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
}

glm::vec3 HDRLight::GetHalfSize() const
{
    return GetScale() / 2.f;
}

void HDRLight::SetHalfSize(const glm::vec3& halfSize)
{
    SetScale(halfSize * 2.f);
}

glm::vec3 HDRLight::GetMin() const
{
    return WorldPosition() - GetHalfSize();
}

glm::vec3 HDRLight::GetMax() const
{
    return WorldPosition() + GetHalfSize();
}

void HDRLight::SetHDRTexture(std::shared_ptr<Asset> hdrTexture)
{
    _SetNeedsUpdate(GetNeedsUpdate() || (hdrTexture != GetHDRTexture()));
    SetComponent(hdrTexture);
}

std::shared_ptr<Asset> HDRLight::GetHDRTexture()
{
    return GetComponent<Asset>();
}

void HDRLight::render_shadow()
{
}

void HDRLight::Draw()
{
    if (GetNeedsUpdate()) {
        _Update();
        Renderer::DeferredLightingBuffer()->bind();
    }
    auto geometryBuffer = Renderer::DeferredGeometryBuffer();
    glm::vec3 geometryPosition;
    if (GetInfinite())
        geometryPosition = Scene::Current()->CurrentCamera()->WorldPosition();
    else
        geometryPosition = GetParent() ? GetParent()->WorldPosition() + GetPosition() : GetPosition();
    _deferredShader->Use()
        .SetTexture("ReflectionMap", GetReflection())
        .SetTexture("DefaultBRDFLUT", Renderer::DefaultBRDFLUT())
        .SetUniform("SH[0]", _SHDiffuse.data(), _SHDiffuse.size())
        .SetUniform("Light.Max", GetMax())
        .SetUniform("Light.Min", GetMin())
        .SetUniform("Light.Infinite", GetInfinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(HDRLightGeometry(), true);
    _deferredShader->Done();
}

void HDRLight::DrawProbe(LightProbe& lightProbe)
{
    _Update();
    for (auto i = 0u; i < _SHDiffuse.size(); ++i)
        lightProbe.GetDiffuseSH().at(i) += _SHDiffuse.at(i);
    lightProbe.GetReflectionBuffer()->bind();
    _probeShader->Use()
        .SetUniform("Light.Min", GetMin())
        .SetUniform("Light.Max", GetMax())
        .SetUniform("Light.Infinite", GetInfinite())
        .SetUniform("ProbePosition", lightProbe.GetAbsolutePosition())
        .SetTexture("ReflectionMap", GetReflection());
    Renderer::Render(Renderer::DisplayQuad());
}

glm::vec2 ToImageCoords(double phi, double theta, int width, int height) {
    // Allow theta to repeat and map to 0 to pi. However, to account for cases
    // where y goes beyond the normal 0 to pi range, phi may need to be adjusted.
    theta = glm::clamp(glm::mod(theta, 2.0 * M_PI), 0.0, 2.0 * M_PI);
    if (theta > M_PI) {
        // theta is out of bounds. Effectively, theta has rotated past the pole
        // so after adjusting theta to be in range, rotating phi by pi forms an
        // equivalent direction.
        theta = 2.0 * M_PI - theta;  // now theta is between 0 and pi
        phi += M_PI;
    }
    // Allow phi to repeat and map to the normal 0 to 2pi range.
    // Clamp and map after adjusting theta in case theta was forced to update phi.
    phi = glm::clamp(glm::mod(phi, 2.0 * M_PI), 0.0, 2.0 * M_PI);

    // Now phi is in [0, 2pi] and theta is in [0, pi] so it's simple to inverse
    // the linear equations in ImageCoordsToSphericalCoords, although there's no
    // -0.5 because we're returning floating point coordinates and so don't need
    // to center the pixel.
    return glm::vec2(width * phi / (2.0 * M_PI), height * theta / M_PI);
}

const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
glm::vec2 SampleSphericalMap(glm::vec3 xyz)
{
    glm::vec2 uv = glm::vec2(atan2(xyz.z, xyz.x), asin(xyz.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void HDRLight::_Update()
{
    if (!GetNeedsUpdate())
        return;
    auto asset { GetComponent<Asset>() };
    asset->Load();
    assert(asset->GetAssetType() == Image::AssetType);
    auto image { asset->GetComponent<Image>() };
    _SHDiffuse = s_SH.ProjectFunction(
        [=](const SphericalHarmonics::Sample& sample) {
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
    RemoveComponent(asset);
    _SetNeedsUpdate(false);
}
