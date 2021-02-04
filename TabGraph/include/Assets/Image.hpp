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
    READONLYPROPERTY(Pixel::Description, PixelDescription);
    READONLYPROPERTY(glm::ivec2, Size, glm::ivec2(0, 0));

public:
    static constexpr auto AssetType = "Image";
    /**
     * @brief Constructs a new Image instance
     * @param size The resolution in pixels
     * @param pixelDescription See PixelUtils, can be created using SizedFormat
     * @param rawData The pixel data to be used for the image
    */
    Image(const glm::ivec2 size, Pixel::Description pixelDescription, std::vector<std::byte> rawData = {});
    //Image(const Image& other);
    ~Image();
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
    /** @brief Loads the Image synchronously, must emit LoadedChanged(true) if loading was successful */
    //virtual void Load() override;
    /**
     * @brief Loads the Image asynchronously -> returns immediatly.
     * Emits LoadedChanged(true) if loading was successful.
    */
    //virtual void LoadAsync() override;
    /**
     * @brief Loads the Image synchrounsly.
     * Emits LoadedChanged(true) if loading was successful.
    */
    void Free();

    /**
     * @brief Fetches a color from the coordinates, asserts thad _data is not empty
     * @param texCoord the texture coordinate to fetch the color from
     * @return the unpacked color
    */
    glm::vec4 GetColor(glm::ivec2 texCoord);
    /**
     * @brief Sets the pixel corresponding to tesCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color the new color of this pixel
    */
    void SetColor(glm::ivec2 texCoord, glm::vec4 color);

    void SetPixelDescription(Pixel::Description pixelDescription);
    void SetSize(glm::ivec2 size);
    /**
     * @brief
     * @return true if the image is currently loading in an other thread
    */
    std::atomic<bool>& GetLoading();

private:
    void _DoLoad();
    std::byte* _GetPointer(glm::ivec2 texCoord);
    std::atomic<bool> _loading { false };
    std::vector<std::byte> _data;
    std::thread _parsingThread;

    // Hérité via AssetData
    virtual std::shared_ptr<Component> _Clone() override;
};