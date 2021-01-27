#pragma once

#include "Texture/Texture.hpp"
//#include "Render.hpp"

//#include <filesystem>
#include <glm/vec2.hpp>

#define MIPMAPNBR(size) int(log2(std::max(size.x, size.y)))
class Image;

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
    /**
     * @brief Creates a Texture2D with an image to load from
     * the image is released when loading is done with RemoveComponent
     * @param image the image to use for loading, released when loading is done
    */
    Texture2D(std::shared_ptr<Image> image);
    virtual void Load() override;
    void SetSize(glm::ivec2 size);

private:
    std::shared_ptr<Component> _Clone() override {
        return std::static_pointer_cast<Component>(Component::Create<Texture2D>(*this));
    }
    void _AllocateStorage();
    Signal<bool>::ScoppedSlot _onImageLoadedSlot;
    Signal<float>::ScoppedSlot _onBeforeRenderSlot;
    void _OnImageLoaded(bool loaded);
    void _OnBeforeRender(float);
    void _UploadImage(std::shared_ptr<Image> image);
};
