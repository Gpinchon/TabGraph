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
    READONLYPROPERTY(std::shared_ptr<Assets::Asset>, HDRImage, nullptr);
    READONLYPROPERTY(std::shared_ptr<Textures::TextureCubemap>, HDRTexture, nullptr);
    PROPERTY(bool, Infinite, true);

public:
    HDRLight(const std::string& name, std::shared_ptr<Assets::Asset> image);
    inline glm::vec3 GetHalfSize() const
    {
        return GetLocalScale() / 2.f;
    }
    inline void SetHalfSize(const glm::vec3& halfSize)
    {
        SetLocalScale(halfSize * 2.f);
    }
    inline glm::vec3 GetMin() const
    {
        return GetWorldPosition() - GetHalfSize();
    }
    inline glm::vec3 GetMax() const
    {
        return GetWorldPosition() + GetHalfSize();
    }
    void SetHDRImage(std::shared_ptr<Assets::Asset> image);
};

}
