/*
* @Author: gpinchon
* @Date:   2021-03-12 16:08:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 18:26:25
*/

#ifdef IGNORE_SKY
#include <SG/Cameras/Camera.hpp>
#include <SG/Light/SkyLight.hpp>
#include <SG/Renderer/Renderer.hpp>
#include <SG/Renderer/Shapes/GeometryRenderer.hpp>
#include <SG/Nodes/Scene.hpp>
#include <SG/Shader/Program.hpp>
#include <SG/SphericalHarmonics.hpp>
#include <SG/Shapes/Generators/Cube.hpp>
#include <SG/Shapes/Geometry.hpp>
#include <SG/Texture/Texture2D.hpp>
#include <SG/Texture/TextureCubemap.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Light/SkyLightRenderer.hpp>
#endif

#include <glm/vec3.hpp>

#ifndef M_PI
constexpr auto M_PI = 3.14159265358979323846;
#endif // M_PI

constexpr auto num_samples = 16;
constexpr auto num_samples_light = 8;

/// \private
struct ray_t {
    glm::vec3 origin;
    glm::vec3 direction;
};

/// \private
struct sphere_t {
    glm::vec3 origin;
    float radius;
};

bool isect_sphere(const ray_t ray, const sphere_t sphere, float& t0, float& t1)
{
    glm::vec3 rc = sphere.origin - ray.origin;
    float radius2 = sphere.radius * sphere.radius;
    float tca = dot(rc, ray.direction);
    float d2 = dot(rc, rc) - tca * tca;
    if (d2 > radius2)
        return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
}

float rayleigh_phase_func(float mu)
{
    return 3. * (1. + mu * mu)
        / //------------------------
        (16. * M_PI);
}

float henyey_greenstein_phase_func(float mu)
{
    const float g = 0.76;
    return (1. - g * g)
        / //---------------------------------------------
        ((4. * M_PI) * pow(1. + g * g - 2. * g * mu, 1.5));
}

bool get_sun_light(const ray_t& ray, float& optical_depthR, float& optical_depthM, const TabGraph::Lights::SkyLight& sky)
{
    float t0, t1;
    const sphere_t atmosphere = sphere_t{
        glm::vec3(0, 0, 0), sky.GetAtmosphereRadius()
    };
    isect_sphere(ray, atmosphere, t0, t1);

    float march_pos = 0.;
    float march_step = t1 / float(num_samples_light);

    for (int i = 0; i < num_samples_light; i++) {
        glm::vec3 s = ray.origin + ray.direction * float(march_pos + 0.5 * march_step);
        float height = length(s) - sky.GetPlanetRadius();
        if (height < 0.)
            return false;

        optical_depthR += exp(-height / sky.GetHRayleigh()) * march_step;
        optical_depthM += exp(-height / sky.GetHMie()) * march_step;

        march_pos += march_step;
    }

    return true;
}

static inline glm::vec3 GetIncidentLight(ray_t ray, const TabGraph::Lights::SkyLight& sky)
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
        glm::vec3 s = ray.origin + ray.direction * float(march_pos + 0.5 * march_step);
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
            glm::vec3 tau = sky.GetBetaRayleigh() * (optical_depthR + optical_depth_lightR) + sky.GetBetaMie() * 1.1f * (optical_depthM + optical_depth_lightM);
            glm::vec3 attenuation = exp(-tau);

            sumR += hr * attenuation;
            sumM += hm * attenuation;
        }

        march_pos += march_step;
    }

    return sky.GetSunPower() * (sumR * phaseR * sky.GetBetaRayleigh() + sumM * phaseM * sky.GetBetaMie());
}

namespace TabGraph::Lights {
SkyLight::SkyLight()
    : Inherit()
{
}

glm::vec3 SkyLight::GetSunDirection() const
{
    return GetDirection();
}

void SkyLight::SetSunDirection(const glm::vec3& sunDir)
{
    if (sunDir != GetSunDirection())
        GetRenderer().FlagDirty();
    SetDirection(sunDir);
}

void SkyLight::SetSunPower(const float sunPower)
{
    if (sunPower != GetSunPower())
        GetRenderer().FlagDirty();
    _SetSunPower(sunPower);
}

void SkyLight::SetPlanetRadius(float planetRadius)
{
    if (planetRadius != GetPlanetRadius())
        GetRenderer().FlagDirty();
    _SetPlanetRadius(planetRadius);
}

void SkyLight::SetAtmosphereRadius(float atmosphereRadius)
{
    if (atmosphereRadius != GetAtmosphereRadius())
        GetRenderer().FlagDirty();
    _SetAtmosphereRadius(atmosphereRadius);
}

void SkyLight::SetHRayleigh(float hRayleigh)
{
    if (hRayleigh != GetHRayleigh())
        GetRenderer().FlagDirty();
    _SetHRayleigh(hRayleigh);
}

void SkyLight::SetHMie(float hMie)
{
    if (hMie != GetHMie())
        GetRenderer().FlagDirty();
    _SetHMie(hMie);
}

void SkyLight::SetBetaRayleigh(glm::vec3 betaRayleigh)
{
    if (betaRayleigh != GetBetaRayleigh())
        GetRenderer().FlagDirty();
    _SetBetaRayleigh(betaRayleigh);
}

void SkyLight::SetBetaMie(glm::vec3 betaMie)
{
    if (betaMie != GetBetaMie())
        GetRenderer().FlagDirty();
    _SetBetaMie(betaMie);
}

glm::vec3 SkyLight::GetIncidentLight(glm::vec3 direction) const
{
    return ::GetIncidentLight({
        glm::vec3(0, GetPlanetRadius() + 1, 0), direction },
        *this
    );
}
}
#endif