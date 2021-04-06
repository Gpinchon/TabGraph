#pragma once

#include "Texture/Texture.hpp"

#include <glm/vec2.hpp>

#define MIPMAPNBR(size) int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(std::max(size.x, size.y))))
class Asset;

class Texture2D : public Texture {
    READONLYPROPERTY(glm::ivec2, Size, 0);
    PROPERTY(bool, AutoMipMap, true);
    PROPERTY(int, MipMapNbr, MIPMAPNBR(GetSize()));

public:
    /**
     * @param size : the resolutionin pixels
     * @param internalFormat : the sized format that will be used to store the texture on the GPU
    */
    Texture2D(glm::ivec2 size, Pixel::SizedFormat internalFormat);
    Texture2D(glm::ivec2 size, Pixel::SizedFormat internalFormat, uint8_t multiSample);
    /**
     * @brief Creates a Texture2D with an image to load from
     * the image is released when loading is done with RemoveComponent
     * @param image the image to use for loading, released when loading is done
    */
    Texture2D(std::shared_ptr<Asset> image);
    Texture2D(std::shared_ptr<Asset> image, uint8_t multiSample);
    virtual void Load() override;
    void SetSize(glm::ivec2 size);

private:
    std::shared_ptr<Component> _Clone() override {
        auto clone{ Component::Create<Texture2D>(*this) };
        clone->_SetHandle(0);
        clone->_SetLoaded(false);
        return std::static_pointer_cast<Component>(clone);
    }
    void _AllocateStorage();
    void _UploadImage(std::shared_ptr<Asset> imageAsset);
};
