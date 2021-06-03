/*
* @Author: gpinchon
* @Date:   2021-01-07 17:35:23
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-12 18:24:28
*/
#pragma once

#include "Texture/PixelUtils.hpp"
#include "Component.hpp"
#include "Event/Signal.hpp"

#include <filesystem>
#include <glm/glm.hpp>
#include <thread>
#include <vector>
constexpr auto constString = "constString";

/**
 * @brief Describes an image file located at the path used in the constructor
*/
class Image : public Component {
public:
    enum class SamplingFilter {
        Nearest,
        Bilinear,
        MaxValue
    };
    READONLYPROPERTY(Pixel::Description, PixelDescription);

public:
    static constexpr auto AssetType = "Image";
    /**
     * @brief Constructs a new Image instance
     * @param size The resolution in pixels
     * @param pixelDescription See PixelUtils, can be created using SizedFormat
     * @param rawData The pixel data to be used for the image
    */
    Image(const glm::ivec2& size, Pixel::Description pixelDescription, const std::vector<std::byte>& rawData = {});
    /**
     * @brief 
     * @param data the new unpacked data, data size must be : Size.x * Size.y * Octets Per Pixel
    */
    void SetData(const std::vector<std::byte>& data);
    /**
     * @brief Returns a reference to the Image's raw data
     * @return the image's raw data, data is formated to the PixelFormat
    */
    std::vector<std::byte>& GetData();
    /**
     * @brief Fetches a color from the coordinates, asserts thad _data is not empty
     * @param texCoord the texture coordinate to fetch the color from
     * @param filter the filtering to be used for sampling, default is nearest
     * @return the unpacked color
    */
    glm::vec4 GetColor(const glm::vec2& texCoord, SamplingFilter filter = SamplingFilter::Nearest);
    /**
     * @brief Sets the pixel corresponding to tesCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
    */
    void SetColor(const glm::ivec2& texCoord, const glm::vec4& color);
    /**
     * @brief Sets the pixel description of this image
     * @param pixelDescription : the new pixel format of this image
    */
    void SetPixelDescription(Pixel::Description pixelDescription);
    /**
     * @brief Sets the size in pixels of this image
     * @param size : the new size of this image
     * @param filter : the filter to be used for resizing
    */
    void SetSize(const glm::ivec2& size, SamplingFilter filter = SamplingFilter::Nearest);
    /**
     * @brief This image's size
     * @return the image's size
    */
    glm::ivec2 GetSize() const;

private:
    std::byte* _GetPointer(glm::ivec2 texCoord);
    std::vector<std::byte> _data;
    glm::ivec2 _size{ 0 };

    // Hérité via AssetData
    virtual std::shared_ptr<Component> _Clone() override {
        return std::static_pointer_cast<Component>(shared_from_this());
    }
};