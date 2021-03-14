/*
* @Author: gpinchon
* @Date:   2021-03-12 16:08:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-12 18:33:21
*/

#include "Light/Sky.hpp"
#include "Camera/Camera.hpp"
#include "Framebuffer.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Render.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Program.hpp"
#include "SphericalHarmonics.hpp"
#include "Texture/Texture2D.hpp"

struct ray_t {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct sphere_t {
    glm::vec3 origin;
    float radius;
};

bool isect_sphere(ray_t ray, sphere_t sphere, float& t0, float& t1)
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

bool get_sun_light(ray_t ray, float& optical_depthR, float& optical_depthM, const Sky& sky)
{
    const int num_samples_light = 8;
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


glm::vec3 get_incident_light(ray_t ray, const Sky& sky)
{
    const int num_samples = 16;
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
    float mu = glm::dot(ray.direction, sky.GetSunDir());

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
            sky.GetSunDir()
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

static inline auto SkyLightVertexCode()
{
    static auto lightingVertexCode =
#include "light.vert"
        ;
    static Shader::Stage::Code shaderCode{ lightingVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto SkyLightFragmentCode()
{
    static auto deferred_frag_code =
#include "deferred.frag"
        ;
    static auto SHCode =
#include "sphericalHarmonics.glsl"
        ;
    static auto lightingFragmentCode =
#include "SkyLight.frag"
        ;
    static Shader::Stage::Code shaderCode =
        Shader::Stage::Code{ deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code{ SHCode } +
        Shader::Stage::Code{ lightingFragmentCode, "Lighting();" };
    return shaderCode;
}

auto inline SkyLightShader()
{
    static std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        shader = Component::Create<Shader::Program>("DirectionnalLightShader");
        shader->SetDefine("LIGHTSHADER");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, SkyLightVertexCode()));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, SkyLightFragmentCode()));
    }
    return shader;
}

static inline auto SkyLightGeometry()
{
    static auto geometry = CubeMesh::CreateGeometry("SkyLightGeometry", glm::vec3(1));
    return geometry;
}

Sky::Sky()
{
    if (_GetShader() == nullptr) {
        _SetShader(Component::Create<Shader::Program>("SkyLightShader"));
        _GetShader()->SetDefine("LIGHTSHADER");
        _GetShader()->Attach(Shader::Stage(Shader::Stage::Type::Vertex, SkyLightVertexCode()));
        _GetShader()->Attach(Shader::Stage(Shader::Stage::Type::Fragment, SkyLightFragmentCode()));
    }
}

void Sky::Draw()
{
    static auto SH{ SphericalHarmonics(100) };
    if (_SHNeedsUpdate) {
        _sphericalHarmonics = SH.ProjectFunction(
            [=](const SphericalHarmonics::Sample& sample) {
                return get_incident_light({
                           glm::vec3(0, GetPlanetRadius() + 1, 0),
                           sample.vec
                    }, *this);

            }
        );
        _SHNeedsUpdate = false;
    }
    auto geometryBuffer = Render::GeometryBuffer();
    glm::vec3 geometryPosition;
    if (Infinite())
        geometryPosition = Scene::Current()->CurrentCamera()->WorldPosition();
    else
        geometryPosition = GetParent() ? GetParent()->WorldPosition() + GetPosition() : GetPosition();
    if (GetCastShadow())
        _GetShader()->SetDefine("SHADOW");
    else
        _GetShader()->RemoveDefine("SHADOW");
    _GetShader()->Use()
        //.SetTexture("SpecularLUT", _specularLUT)
        .SetTexture("DefaultBRDFLUT", Render::DefaultBRDFLUT())
        .SetTexture("Light.Shadow", GetCastShadow() ? _GetShadowBuffer()->depth() : nullptr)
        .SetUniform("SphericalHarmonics[0]", _sphericalHarmonics.data(), _sphericalHarmonics.size())
        .SetUniform("SHCartesianShapes[0]", SH.GetCartesianCoeffs().data(), SH.GetCartesianCoeffs().size())
        .SetUniform("Light.Max", Max())
        .SetUniform("Light.Min", Min())
        .SetUniform("Light.Projection", (Infinite() ? ShadowProjectionMatrixInfinite() : ShadowProjectionMatrixFinite()) * ShadowViewMatrix())
        .SetUniform("Light.Color", GetColor())
        .SetUniform("Light.Direction", GetDirection())
        .SetUniform("Light.Infinite", Infinite())
        .SetUniform("Matrix.Model", glm::translate(geometryPosition) * GetLocalScaleMatrix())
        .SetTexture("Texture.Geometry.CDiff", geometryBuffer->attachement(0))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->attachement(2))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->attachement(4))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->depth());
    SkyLightGeometry()->Draw();
    _GetShader()->Done();
}

glm::vec3 Sky::GetSunDir() const
{
    return GetDirection();
}

void Sky::SetSunDir(const glm::vec3& sunDir)
{
    _SHNeedsUpdate |= sunDir != GetDirection();
    SetDirection(sunDir);
}

float Sky::GetSunPower() const
{
    return _sunPower;
}

void Sky::SetSunPower(const float sunPower)
{
    _SHNeedsUpdate |= sunPower != _sunPower;
    _sunPower = sunPower;
}

float Sky::GetPlanetRadius() const
{
    return _planetRadius;
}

void Sky::SetPlanetRadius(float planetRadius)
{
    _SHNeedsUpdate |= planetRadius != _planetRadius;
    _planetRadius = planetRadius;
}

float Sky::GetAtmosphereRadius() const
{
    return _atmosphereRadius;
}

void Sky::SetAtmosphereRadius(float atmosphereRadius)
{
    _SHNeedsUpdate |= atmosphereRadius != _atmosphereRadius;
    _atmosphereRadius = atmosphereRadius;
}

float Sky::GetHRayleigh() const
{
    return _hRayleigh;
}

void Sky::SetHRayleigh(float hRayleigh)
{
    _SHNeedsUpdate |= hRayleigh != _hRayleigh;
    _hRayleigh = hRayleigh;
}

float Sky::GetHMie() const
{
    return _hMie;
}

void Sky::SetHMie(float hMie)
{
    _SHNeedsUpdate |= hMie != _hMie;
    _hMie = hMie;
}

glm::vec3 Sky::GetBetaRayleigh() const
{
    return _betaRayleigh;
}

void Sky::SetBetaRayleigh(glm::vec3 betaRayleigh)
{
    _SHNeedsUpdate |= betaRayleigh != _betaRayleigh;
    _betaRayleigh = betaRayleigh;
}

glm::vec3 Sky::GetBetaMie() const
{
    return _betaMie;
}

void Sky::SetBetaMie(glm::vec3 betaMie)
{
    _SHNeedsUpdate |= betaMie != _betaMie;
    _betaMie = betaMie;
}
