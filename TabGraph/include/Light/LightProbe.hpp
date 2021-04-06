/*
* @Author: gpinchon
* @Date:   2021-03-16 19:54:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-18 01:21:05
*/

#pragma once

#include "Object.hpp"
#include "SphericalHarmonics.hpp"

#include <glm/glm.hpp>
#include <vector>

class Framebuffer;

class LightProbeGroup;

class LightProbe : public Object {
    PROPERTY(bool, Infinite, 0);
    PROPERTY(std::shared_ptr<Framebuffer>, ReflectionBuffer, nullptr);
    PROPERTY(glm::vec3, Position, 0);

public:
    LightProbe(LightProbeGroup& lightProbeGroup);

    std::vector<glm::vec3>& GetDiffuseSH() {
        return _diffuseSH;
    }
    const SphericalHarmonics& GetSphericalHarmonics() const;

    glm::vec3 GetAbsolutePosition() const;

private:
    const LightProbeGroup& _lightProbeGroup;
    std::vector<glm::vec3> _diffuseSH{ 16 };
};

class LightProbeGroup : public Object {
    friend LightProbe;
    PROPERTY(glm::vec3, Position, 0);
    READONLYPROPERTY(glm::ivec2, Resolution, 128);

public:
    LightProbeGroup(size_t probesNbr, glm::ivec2 resolution = glm::ivec2(128)) : _lightProbes(probesNbr, *this) {
        _SetResolution(resolution);
    }
    std::vector<LightProbe> &GetLightProbes();

private:
    std::vector<LightProbe> _lightProbes;
    SphericalHarmonics _sphericalHarmonics{ 50 };
};
