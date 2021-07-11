/*
* @Author: gpinchon
* @Date:   2021-03-14 22:12:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Light/Light.hpp>
#include <SphericalHarmonics.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shader {
class Program;
}
namespace Textures {
class TextureCubemap;
}
namespace Assets {
class Asset;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Lights {
class HDRLight : public Core::Inherit<Light, HDRLight> {
    READONLYPROPERTY(std::shared_ptr<Textures::TextureCubemap>, Reflection, nullptr);
    PROPERTY(bool, Infinite, true);

public:
    HDRLight(std::shared_ptr<Assets::Asset> hdrTexture);
    glm::vec3 GetHalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 GetMin() const;
    glm::vec3 GetMax() const;
    void SetHDRTexture(std::shared_ptr<Assets::Asset> hdrTexture);
    std::shared_ptr<Assets::Asset> GetHDRTexture();
private:
    std::shared_ptr<Assets::Asset> _HDRTexture;
};

}
