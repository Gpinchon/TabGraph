/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

#pragma once

#include "Texture.hpp" // for Texture

#include <glm/vec2.hpp>
#include <array> // for array
#include <filesystem>
#include <memory> // for shared_ptr, shared_ptr
#include <string> // for string
#include <vector> // for vector

#define MIPMAPNBR(size) int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(std::max(size.x, size.y))))

class Asset;
class Image;

class TextureCubemap : public Texture {
public:
    class Impl;
    friend Impl;
    enum class Side {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };
    TextureCubemap() = delete;
    TextureCubemap(const TextureCubemap&);
    TextureCubemap(glm::ivec2 size, Pixel::SizedFormat format);
    TextureCubemap(std::shared_ptr<Asset> image);
    ~TextureCubemap();
    /**
     * @brief extracts TextureCubemap's side from equirectangular image
     * @param fromImage the equirectangular image to be used
     * @param toImage the face to extract
     *
     */
    static void ExtractSide(std::shared_ptr<Image> fromImage, std::shared_ptr<Image> toImage, TextureCubemap::Side side);
    glm::ivec2 GetSize() const;
    void SetSize(glm::ivec2 size);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<TextureCubemap>(*this);
    }
};