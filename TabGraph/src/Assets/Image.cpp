/*
* @Author: gpinchon
* @Date:   2021-01-11 08:40:24
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 17:25:19
*/

#include "Assets/Image.hpp"

#include <glm/glm.hpp>
#include <thread>
#include <unordered_map>

static size_t s_imageNbr = 0;

Image::Image(const glm::ivec2 size, Pixel::Description pixelDescription, std::vector<std::byte> rawData) : Component("Image_" + std::to_string(s_imageNbr)), _data(rawData)// : Asset("data:image_" + std::to_string(s_imageNbr))
{
    SetPixelDescription(pixelDescription);
    SetSize(size);
    auto rawDataSize{ size.x * size.y * GetPixelDescription().GetSize() };
    if (!rawData.empty())
        assert(rawData.size() == rawDataSize);
    _data.resize(rawDataSize);
}

Image::~Image()
{
    if (_parsingThread.joinable())
        _parsingThread.join();
}

void Image::SetData(const std::vector<std::byte>& data)
{
    _data = data;
}

std::vector<std::byte>& Image::GetData()
{
    return _data;
}

glm::vec4 Image::GetColor(glm::ivec2 texCoord)
{
    assert(!GetData().empty() && "Image::GetColor : Unpacked Data is empty");
    return GetPixelDescription().GetColorFromBytes(_GetPointer(texCoord));
}

void Image::SetColor(glm::ivec2 texCoord, glm::vec4 color)
{
    assert(!GetData().empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(texCoord), color);
}

void Image::SetPixelDescription(Pixel::Description pixelFormat)
{
    _SetPixelDescription(pixelFormat);
}

void Image::SetSize(glm::ivec2 size)
{
    _SetSize(size);
}

std::byte* Image::_GetPointer(glm::ivec2 texCoord)
{
    size_t pitch{ static_cast<size_t>(GetSize().x) * GetPixelDescription().GetSize() };
    size_t index{
        texCoord.y * pitch +
        texCoord.x * GetPixelDescription().GetSize()
    };
    assert(index < GetData().size() && "Image::_GetPointer : Unpacked Data index out of bound");
    return GetData().data() + index;
}
