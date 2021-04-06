/*
* @Author: gpinchon
* @Date:   2021-03-12 16:01:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:29:22
*/

#include "Light/DirectionnalLight.hpp"
#include "SphericalHarmonics.hpp"

class Cubemap;

class SkyLight : public DirectionnalLight {
public:
    SkyLight();
    glm::vec3 GetSunDirection() const;
    void SetSunDirection(const glm::vec3& dir);
    float GetSunPower() const;
    void SetSunPower(const float power);
    /**
     * @return Planet's radius in meters
    */
    float GetPlanetRadius() const;
    void SetPlanetRadius(float radius);
    /**
     * @return Planet's radius in meters
    */
    float GetAtmosphereRadius() const;
    void SetAtmosphereRadius(float radius);
    /**
     * @return Rayleigh scale heigh
    */
    float GetHRayleigh() const;
    void SetHRayleigh(float hRayleigh);
    /**
     * @return Mie scale heigh
    */
    float GetHMie() const;
    void SetHMie(float hMie);
    /**
     * @return Rayleigh's scattering coefficient at sea level
    */
    glm::vec3 GetBetaRayleigh() const;
    void SetBetaRayleigh(glm::vec3 betaRayleigh);
    /**
     * @return Mie's scattering coefficient at sea level
    */
    glm::vec3 GetBetaMie() const;
    void SetBetaMie(glm::vec3 betaMie);

    virtual void Draw() override;
    virtual void DrawProbe(LightProbe& lightProbe) override;

private:
    void _UpdateLUT();
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Cubemap> _reflectionLUT;
    float _sunPower { 20.f };
    float _planetRadius { 6360e3 };
    float _atmosphereRadius { 6420e3 };
    float _hRayleigh { 7994 };
    float _hMie { 1200 };
    glm::vec3 _betaRayleigh { 5.5e-6, 13.0e-6, 22.4e-6 };
    glm::vec3 _betaMie { 21e-6 };
};