/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:03
*/

#pragma once

#include "Texture2D.hpp" // for Texture2D

#include <array> // for array
#include <filesystem>
#include <memory> // for shared_ptr, shared_ptr
#include <string> // for string
#include <vector> // for vector

class Image;

class Cubemap : public Texture2D {
public:
    enum class Side {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };
    Cubemap() = delete;
    Cubemap(glm::ivec2 size, Pixel::SizedFormat format);
    Cubemap(std::shared_ptr<Asset> fromImage);
    ~Cubemap();
    virtual void Load() override;
    /**
     * @brief extracts Cubemap's side from equirectangular image
     * @param fromImage the equirectangular image to be used
     * @param toImage the face to extract
     *
     */
    static void ExtractSide(std::shared_ptr<Image> fromImage, std::shared_ptr<Image> toImage, Cubemap::Side side);

private:
    virtual void _Allocate();
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Cubemap>(*this);
    }
};