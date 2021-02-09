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

//Image::Image(const std::filesystem::path& filePath) : Asset(filePath)
//{
//}
//const std::string Image::AssetType = "Image";
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

//Image::Image(const Image& other) : AssetData(other)
//{
//    _PixelDescription = other._PixelDescription;
//    _Size = other._Size;
//    _data = other._data;
//}

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

//void Image::Load()
//{
//    bool isLoading{ true };
//    GetLoading().compare_exchange_weak(isLoading, true);
//    if (isLoading) //We are loading from another thread
//        _parsingThread.join();
//    if (GetLoaded()) //Image already loaded
//        return;
//    _DoLoad();
//}

//void Image::LoadAsync()
//{
//    bool isLoading{ true };
//    GetLoading().compare_exchange_weak(isLoading, true);
//    if (isLoading) //We are loading from another thread
//        return;
//        //_parsingThread.join();
//    if (GetLoaded()) //Image already loaded
//        return;
//    if (!GetLoaded())
//        _parsingThread = std::thread(&Image::_DoLoad, this);
//}

//void Image::Free()
//{
//    //We're loading this image from an other thread or it's not been loaded yet, don't free it.
//    if (!GetLoaded() || GetLoading()) 
//        return;
//    GetData().clear();
//    GetData().shrink_to_fit();
//    SetLoaded(false);
//}

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
    //SetLoaded(false);
}

void Image::SetSize(glm::ivec2 size)
{
    _SetSize(size);
    //SetLoaded(false);
}

//void Image::_DoLoad()
//{
//    _loading = true;
//    ImageParser::Parse(std::static_pointer_cast<Image>(shared_from_this()));
//    _loading = false;
//}

std::byte* Image::_GetPointer(glm::ivec2 texCoord)
{
    size_t pitch{ static_cast<size_t>(GetSize().x) * GetPixelDescription().GetSize() };
    size_t index{
        texCoord.y * pitch +
        texCoord.x * GetPixelDescription().GetSize()
    };
    //size_t index = (static_cast<size_t>(texCoord.x) * GetSize().y + texCoord.y) * GetPixelDescription().GetSize();
    assert(index < GetData().size() && "Image::_GetPointer : Unpacked Data index out of bound");
    return GetData().data() + index;
}

std::atomic<bool>& Image::GetLoading()
{
    return _loading;
}
