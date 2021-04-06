/*
* @Author: gpinchon
* @Date:   2021-03-14 22:12:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-18 17:20:58
*/
#pragma once

#include "Light/Light.hpp"
#include "SphericalHarmonics.hpp"

namespace Shader {
class Program;
}

class Cubemap;
class Asset;

class HDRLight : public Light {
    READONLYPROPERTY(std::shared_ptr<Cubemap>, Reflection, nullptr);
    //READONLYPROPERTY(std::vector<glm::vec3>, DiffuseSH, 0);
    READONLYPROPERTY(bool, NeedsUpdate, true);
    PROPERTY(bool, Infinite, true);

public:
    HDRLight(std::shared_ptr<Asset> hdrTexture);
    glm::vec3 GetHalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 GetMin() const;
    glm::vec3 GetMax() const;
    void SetHDRTexture(std::shared_ptr<Asset> hdrTexture);
    std::shared_ptr<Asset> GetHDRTexture();

    virtual void render_shadow() override;
    virtual void Draw() override;
    virtual void DrawProbe(LightProbe& lightProbe) override;

private:
    void _Update();
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
};
