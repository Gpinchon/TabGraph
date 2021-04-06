/*
* @Author: gpinchon
* @Date:   2021-03-12 16:08:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-12 18:33:21
*/

#include "Light/SkyLight.hpp"
#include "Camera/Camera.hpp"
#include "Framebuffer.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Program.hpp"
#include "SphericalHarmonics.hpp"
#include "Texture/Texture2D.hpp"
#include "Texture/Cubemap.hpp"

static SphericalHarmonics s_SH{ 50 };

#define num_samples 16
#define num_samples_light 8

struct ray_t {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct sphere_t {
    glm::vec3 origin;
    float radius;
};

bool isect_sphere(const ray_t ray, const sphere_t sphere, float &t0, float &t1)
{
    glm::vec3 rc = sphere.origin - ray.origin;
    float radius2 = sphere.radius * sphere.radius;
    float tca = dot(rc, ray.direction);
    float d2 = dot(rc, rc) - tca * tca;
    if (d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
}

float rayleigh_phase_func(float mu)
{
    return
        3. * (1. + mu * mu)
        / //------------------------
        (16. * M_PI);
}

float henyey_greenstein_phase_func(float mu)
{
    const float g = 0.76;
    return
        (1. - g * g)
        / //---------------------------------------------
        ((4. * M_PI) * pow(1. + g * g - 2. * g * mu, 1.5));
}

bool get_sun_light(const ray_t &ray, float& optical_depthR, float& optical_depthM, const SkyLight& sky)
{
    float t0, t1;
    const sphere_t atmosphere = sphere_t{
        glm::vec3(0, 0, 0), sky.GetAtmosphereRadius()
    };
    isect_sphere(ray, atmosphere, t0, t1);

    float march_pos = 0.;
    float march_step = t1 / float(num_samples_light);

    for (int i = 0; i < num_samples_light; i++) {
        glm::vec3 s =
            ray.origin +
            ray.direction * float(march_pos + 0.5 * march_step);
        float height = length(s) - sky.GetPlanetRadius();
        if (height < 0.)
            return false;

        optical_depthR += exp(-height / sky.GetHRayleigh()) * march_step;
        optical_depthM += exp(-height / sky.GetHMie()) * march_step;

        march_pos += march_step;
    }

    return true;
}

glm::vec3 get_incident_light(ray_t ray, const SkyLight& sky)
{
    const sphere_t atmosphere = sphere_t{
        glm::vec3(0, 0, 0), sky.GetAtmosphereRadius()
    };
    // "pierce" the atmosphere with the viewing ray
    float t0, t1;
    if (!isect_sphere(
        ray, atmosphere, t0, t1)) {
        return glm::vec3(0);
    }

    float march_step = t1 / float(num_samples);

    // cosine of angle between view and light directions
    float mu = glm::dot(ray.direction, sky.GetSunDirection());

    // Rayleigh and Mie phase functions
    // A black box indicating how light is interacting with the material
    // Similar to BRDF except
    // * it usually considers a single angle
    //   (the phase angle between 2 directions)
    // * integrates to 1 over the entire sphere of directions
    float phaseR = rayleigh_phase_func(mu);
    float phaseM = henyey_greenstein_phase_func(mu);

    // optical depth (or "average density")
    // represents the accumulated extinction coefficients
    // along the path, multiplied by the length of that path
    float optical_depthR = 0.;
    float optical_depthM = 0.;

    glm::vec3 sumR = glm::vec3(0);
    glm::vec3 sumM = glm::vec3(0);
    float march_pos = 0.;

    for (int i = 0; i < num_samples; i++) {
        glm::vec3 s =
            ray.origin +
            ray.direction * float(march_pos + 0.5 * march_step);
        float height = glm::length(s) - sky.GetPlanetRadius();

        // integrate the height scale
        float hr = exp(-height / sky.GetHRayleigh()) * march_step;
        float hm = exp(-height / sky.GetHMie()) * march_step;
        optical_depthR += hr;
        optical_depthM += hm;

        // gather the sunlight
        ray_t light_ray{
            s,
            sky.GetSunDirection()
        };
        float optical_depth_lightR = 0.;
        float optical_depth_lightM = 0.;
        bool overground = get_sun_light(
            light_ray,
            optical_depth_lightR,
            optical_depth_lightM,
            sky);

        if (overground) {
            glm::vec3 tau =
                sky.GetBetaRayleigh() * (optical_depthR + optical_depth_lightR) +
                sky.GetBetaMie() * 1.1f * (optical_depthM + optical_depth_lightM);
            glm::vec3 attenuation = exp(-tau);

            sumR += hr * attenuation;
            sumM += hm * attenuation;
        }

        march_pos += march_step;
    }

    return
        sky.GetSunPower() *
        (sumR * phaseR * sky.GetBetaRayleigh() +
         sumM * phaseM * sky.GetBetaMie());
}

static inline auto DeferredSkyLightVertexCode()
{
    auto lightingVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    static Shader::Stage::Code shaderCode{ lightingVertexCode, "TransformGeometry();" };
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
    Shader::Stage::Code shaderCode =
        Shader::Stage::Code{ deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code{ SHCode } +
        Shader::Stage::Code{ lightingFragmentCode, "Lighting();" };
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

SkyLight::SkyLight()
{
    _deferredShader = Component::Create<Shader::Program>("SkyLightShader");
    _deferredShader->SetDefine("Pass", "DeferredLighting");
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredSkyLightVertexCode()));
    _deferredShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, DeferredSkyLightFragmentCode()));
}

glm::vec3 SkyLight::GetSunDirection() const
{
    return GetDirection();
}

void SkyLight::SetSunDirection(const glm::vec3& sunDir)
{
    _needsUpdate |= sunDir != GetSunDirection();
    SetDirection(sunDir);
}

float SkyLight::GetSunPower() const
{
    return _sunPower;
}

void SkyLight::SetSunPower(const float sunPower)
{
    _needsUpdate |= sunPower != _sunPower;
    _sunPower = sunPower;
}

float SkyLight::GetPlanetRadius() const
{
    return _planetRadius;
}

void SkyLight::SetPlanetRadius(float planetRadius)
{
    _needsUpdate |= planetRadius != _planetRadius;
    _planetRadius = planetRadius;
}

float SkyLight::GetAtmosphereRadius() const
{
    return _atmosphereRadius;
}

void SkyLight::SetAtmosphereRadius(float atmosphereRadius)
{
    _needsUpdate |= atmosphereRadius != _atmosphereRadius;
    _atmosphereRadius = atmosphereRadius;
}

float SkyLight::GetHRayleigh() const
{
    return _hRayleigh;
}

void SkyLight::SetHRayleigh(float hRayleigh)
{
    _needsUpdate |= hRayleigh != _hRayleigh;
    _hRayleigh = hRayleigh;
}

float SkyLight::GetHMie() const
{
    return _hMie;
}

void SkyLight::SetHMie(float hMie)
{
    _needsUpdate |= hMie != _hMie;
    _hMie = hMie;
}

glm::vec3 SkyLight::GetBetaRayleigh() const
{
    return _betaRayleigh;
}

void SkyLight::SetBetaRayleigh(glm::vec3 betaRayleigh)
{
    _needsUpdate |= betaRayleigh != _betaRayleigh;
    _betaRayleigh = betaRayleigh;
}

glm::vec3 SkyLight::GetBetaMie() const
{
    return _betaMie;
}

void SkyLight::SetBetaMie(glm::vec3 betaMie)
{
    _needsUpdate |= betaMie != _betaMie;
    _betaMie = betaMie;
}

void SkyLight::Draw()
{
    if (_needsUpdate) {
        _UpdateLUT();
        //re-bind lighting buffer
        Renderer::DeferredLightingBuffer()->bind();
    }
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
    _deferredShader->Use()
        .SetTexture("SpecularLUT", _reflectionLUT)
        .SetTexture("DefaultBRDFLUT", Renderer::DefaultBRDFLUT())
        .SetUniform("SH[0]", _SHDiffuse.data(), _SHDiffuse.size())
        .SetTexture("Light.Shadow", GetCastShadow() ? _GetShadowBuffer()->GetDepthBuffer() : nullptr)
        .SetUniform("Light.SpecularPower", GetSpecularPower())
        .SetUniform("Light.Max", GetMax())
        .SetUniform("Light.Min", GetMin())
        .SetUniform("Light.Projection", (GetInfinite() ? ShadowProjectionMatrixInfinite() : ShadowProjectionMatrixFinite()) * ShadowViewMatrix())
        .SetUniform("Light.Infinite", GetInfinite())
        .SetUniform("GeometryMatrix", glm::translate(geometryPosition) * GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(SkyLightGeometry(), true);
    _deferredShader->Done();
}

void SkyLight::DrawProbe(LightProbe& lightProbe)
{
    _UpdateLUT();
}

void SkyLight::_UpdateLUT()
{
    if (!_needsUpdate)
        return;
    _SHDiffuse = s_SH.ProjectFunction(
        [=](const SphericalHarmonics::Sample& sample) {
            return min(get_incident_light({
                        glm::vec3(0, GetPlanetRadius() + 1, 0),
                        sample.vec
                }, *this), 1.f);
        }
    );
    if (_reflectionLUT == nullptr) {
        _reflectionLUT = Component::Create<Cubemap>(glm::ivec2(256), Pixel::SizedFormat::Float16_RGB);
    }
    static auto s_diffuseLUTBuffer = Component::Create<Framebuffer>("DiffuseLUTBuffer", glm::ivec2(256));
    glDisable(GL_CULL_FACE);
    s_diffuseLUTBuffer->SetColorBuffer(_reflectionLUT, 0);
    s_diffuseLUTBuffer->bind();
    SkyLightLUTShader()->Use()
        .SetUniform("BetaMie", GetBetaMie())
        .SetUniform("BetaRayleigh", GetBetaRayleigh())
        .SetUniform("SunDirection", GetSunDirection())
        .SetUniform("SunPower", GetSunPower())
        .SetUniform("PlanetRadius", GetPlanetRadius())
        .SetUniform("AtmosphereRadius", GetAtmosphereRadius())
        .SetUniform("HRayLeigh", GetHRayleigh())
        .SetUniform("HMie", GetHMie());
    glClear(GL_COLOR_BUFFER_BIT);
    Renderer::Render(Renderer::DisplayQuad());
    Framebuffer::bind_default();
    s_diffuseLUTBuffer->SetColorBuffer(nullptr, 0);
    _reflectionLUT->GenerateMipmap();
    _reflectionLUT->SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::LinearMipmapLinear);
    _needsUpdate = false;
}
