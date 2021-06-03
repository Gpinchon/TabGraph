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

Image::Image(const glm::ivec2& size, Pixel::Description pixelDescription, const std::vector<std::byte>& rawData)
    : Component("Image_" + std::to_string(++s_imageNbr))
    , _data(rawData)
    , _size(size)
{
    SetPixelDescription(pixelDescription);
    auto rawDataSize{ size.x * size.y * GetPixelDescription().GetSize() };
    if (!rawData.empty())
        assert(rawData.size() == rawDataSize);
    _data.resize(rawDataSize);
}

void Image::SetData(const std::vector<std::byte>& data)
{
    _data = data;
}

std::vector<std::byte>& Image::GetData()
{
    return _data;
}

#define CLAMPX(texX) glm::clamp(int(texX), 0, GetSize().x - 1)
#define CLAMPY(texY) glm::clamp(int(texY), 0, GetSize().y - 1)

glm::vec4 Image::GetColor(const glm::vec2& texCoord, Image::SamplingFilter filter)
{
    assert(!GetData().empty() && "Image::GetColor : Unpacked Data is empty");
    if (filter == Image::SamplingFilter::Nearest)
        return GetPixelDescription().GetColorFromBytes(_GetPointer(texCoord));
    else {
        return Pixel::BilinearFilter(glm::fract(texCoord.x), glm::fract(texCoord.y),
            GetColor(glm::ivec2(texCoord.x, texCoord.y)), GetColor(glm::ivec2(CLAMPX(texCoord.x + 1), texCoord.y)),
            GetColor(glm::ivec2(texCoord.x, CLAMPY(texCoord.y + 1))), GetColor(glm::ivec2(CLAMPX(texCoord.x + 1), CLAMPY(texCoord.y + 1))));
    }
}

void Image::SetColor(const glm::ivec2& texCoord, const glm::vec4& color)
{
    assert(!GetData().empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(texCoord), color);
}

void Image::SetPixelDescription(Pixel::Description pixelFormat)
{
    _SetPixelDescription(pixelFormat);
}

void Image::SetSize(const glm::ivec2& size, SamplingFilter filter)
{
    if (size == GetSize()) return;
    auto newImage = Image(size, GetPixelDescription()); //Create empty image
    for (auto y = 0u; y < size.y; ++y) {
        for (auto x = 0u; x < size.x; ++x) {
            auto u = x / float(size.x);
            auto v = y / float(size.y);
            newImage.SetColor(glm::ivec2(x, y), GetColor(glm::vec2(u * GetSize().x, v * GetSize().y), filter));
        }
    }
    _data = newImage._data;
    _data.shrink_to_fit();
    _size = newImage._size;
}

glm::ivec2 Image::GetSize() const
{
    return _size;
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
