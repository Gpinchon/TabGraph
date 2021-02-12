/*
* @Author: gpinchon
* @Date:   2019-04-04 13:53:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:05
*/

#include "Assets/Image.hpp" // for Image
#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"
#include "Buffer/BufferView.hpp"
#include "Texture/PixelUtils.hpp"  // for SizedFormat, SizedFormat::Uint8_NormalizedRGB, SizedFormat::Uint8_NormalizedRGBA
#include "Debug.hpp" // for debugLog

#include <memory> // for allocator, shared_ptr
#include <SDL_image.h> // for IMG_Load
#include <SDL_pixels.h> // for SDL_PixelFormat, SDL_PIXELFORMAT...
#include <SDL_rwops.h>             // for SDL_RWops
#include <SDL_surface.h> // for SDL_Surface, SDL_UnlockSurface
#include <sstream>
#include <stdlib.h> // for free, malloc, NULL
#include <string>                 // for string

void SDL2ImageParser(const std::shared_ptr<Asset>& asset);

auto SDL2MimesExtensions = {
    AssetsParser::AddMimeExtension("image/tga", ".tga"),
    AssetsParser::AddMimeExtension("image/bmp", ".bmp"),
    AssetsParser::AddMimeExtension("image/x-portable-anymap", ".pnm"),
    AssetsParser::AddMimeExtension("image/x-portable-graymap", ".pgm"),
    AssetsParser::AddMimeExtension("image/x-portable-pixmap", ".ppm"),
    AssetsParser::AddMimeExtension("image/x-xpixmap", ".xpm"),
    AssetsParser::AddMimeExtension("image/xcf", ".xcf"),
    AssetsParser::AddMimeExtension("image/x-pcx", ".pcx"),
    AssetsParser::AddMimeExtension("image/gif", ".gif"),
    AssetsParser::AddMimeExtension("image/jpeg", ".jpg"),
    AssetsParser::AddMimeExtension("image/jpeg", ".jpeg"),
    AssetsParser::AddMimeExtension("image/tiff", ".tif"),
    AssetsParser::AddMimeExtension("image/tiff", ".tiff"),
    AssetsParser::AddMimeExtension("image/x-ilbm", ".lbm"),
    AssetsParser::AddMimeExtension("image/x-ilbm", ".iff"),
    AssetsParser::AddMimeExtension("image/png", ".png")
};

auto SDL2MimesParsers = {
    AssetsParser::Add("image/tga", SDL2ImageParser),
    AssetsParser::Add("image/bmp", SDL2ImageParser),
    AssetsParser::Add("image/x-portable-anymap", SDL2ImageParser),
    AssetsParser::Add("image/x-portable-graymap", SDL2ImageParser),
    AssetsParser::Add("image/x-portable-pixmap", SDL2ImageParser),
    AssetsParser::Add("image/x-xpixmap", SDL2ImageParser),
    AssetsParser::Add("image/xcf", SDL2ImageParser),
    AssetsParser::Add("image/x-pcx", SDL2ImageParser),
    AssetsParser::Add("image/gif", SDL2ImageParser),
    AssetsParser::Add("image/jpeg", SDL2ImageParser),
    AssetsParser::Add("image/jpeg", SDL2ImageParser),
    AssetsParser::Add("image/tiff", SDL2ImageParser),
    AssetsParser::Add("image/tiff", SDL2ImageParser),
    AssetsParser::Add("image/x-ilbm", SDL2ImageParser),
    AssetsParser::Add("image/x-ilbm", SDL2ImageParser),
    AssetsParser::Add("image/png", SDL2ImageParser)
};

#define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
#define SDL_UNLOCKIFMUST(s)       \
    {                             \
        if (SDL_MUSTLOCK(s))      \
            SDL_UnlockSurface(s); \
    }

int invert_surface_vertical(SDL_Surface* surface)
{
    uint8_t* t;
    uint8_t *a, *b;
    uint8_t* last;
    uint16_t pitch;

    if (SDL_LOCKIFMUST(surface) < 0)
        return -2;
    if (surface->h < 2) {
        SDL_UNLOCKIFMUST(surface);
        return 0;
    }
    pitch = surface->pitch;
    t = (uint8_t*)malloc(pitch);
    if (t == NULL) {
        SDL_UNLOCKIFMUST(surface);
        return -2;
    }
    memcpy(t, surface->pixels, pitch);
    a = (uint8_t*)surface->pixels;
    last = a + pitch * (surface->h - 1);
    b = last;
    while (a < b) {
        memcpy(a, b, pitch);
        a += pitch;
        memcpy(b, a, pitch);
        b -= pitch;
    }
    memmove(b, b + pitch, last - b);
    memcpy(last, t, pitch);
    free(t);
    SDL_UNLOCKIFMUST(surface);

    return 0;
}

std::string hexToString(int hex)
{
    std::ostringstream stream;
    stream << std::hex << hex;
    return stream.str();
}

void SDL2ImageParser(const std::shared_ptr<Asset>& asset)
{
    auto uri{ asset->GetUri() };
    SDL_RWops* rwOps{ nullptr };
    std::vector<uint8_t> data;
    if (uri.GetScheme() == "data") {
        data = DataUri(uri).Decode();
        if (data.empty()) {
            auto& bufferView{ asset->GetComponent<BufferView>() };
            if (bufferView != nullptr) { //We're loading raw bytes from a BufferView
                auto dataPtr{ (uint8_t*)bufferView->Get(0, bufferView->GetByteLength()) };
                data = std::vector<uint8_t>(
                    dataPtr, dataPtr + bufferView->GetByteLength()
                    );
            }
        }
        rwOps = SDL_RWFromMem(data.data(), data.size());
    }
    else {
        rwOps = SDL_RWFromFile(uri.GetPath().u8string().c_str(), "rb");
    }
        
    auto surface = IMG_Load_RW(rwOps, 1);
    if (!surface || !surface->format)
        throw std::runtime_error(std::string("Error parsing ") + uri.GetPath().string() + " : " + SDL_GetError());

    debugLog("Image Format before conversion :");
    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));

    auto newSurface = SDL_ConvertSurfaceFormat(surface, surface->format->Amask ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24, 0);
    SDL_FreeSurface(surface);
    surface = newSurface;
    invert_surface_vertical(surface);

    debugLog("Image Format after conversion :");
    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));

    auto pixelFormat = surface->format->Amask ? Pixel::SizedFormat::Uint8_NormalizedRGBA : Pixel::SizedFormat::Uint8_NormalizedRGB;
    auto bufferSize{ surface->h * surface->pitch };
    auto rawData{ std::vector<std::byte>((std::byte*)surface->pixels, (std::byte*)surface->pixels + bufferSize) };
    auto size = glm::ivec2(surface->w, surface->h);
    SDL_FreeSurface(surface);
    auto image = std::shared_ptr<Image>(new Image(size, pixelFormat, rawData));
    asset->SetComponent(image);
    asset->SetAssetType(Image::AssetType);
    asset->SetLoaded(true);
}

//std::map<std::filesystem::path, ImageParser*>* ImageParser::_parsers = nullptr; //std::map<std::string, ImageParser *>();
//
//ImageParser::ImageParser(const std::filesystem::path& format, ImageParsingFunction parsingFunction)
//    : _format(format)
//    , _parsingFunction(parsingFunction)
//{
//    debugLog(format);
//}
//
//ImageParser* ImageParser::Add(const std::filesystem::path& format, ImageParsingFunction parsingFunction)
//{
//    auto parser = new ImageParser(format, parsingFunction);
//    _getParsers()[format] = parser;
//    return parser;
//}
//
//std::map<std::filesystem::path, ImageParser*>& ImageParser::_getParsers()
//{
//    if (_parsers == nullptr)
//        _parsers = new std::map<std::filesystem::path, ImageParser*>;
//    return *_parsers;
//}
//
//std::shared_ptr<Image> ImageParser::Parse(const std::filesystem::path& path)
//{
//    auto image = Component::Create<Image>(path);
//    Parse(image);
//    return image;
//}
//
//void ImageParser::Parse(std::shared_ptr<Image> image)
//{
//    auto format = image->GetPath().extension();
//    debugLog(image->GetPath());
//    debugLog(format);
//    auto parser = _get(format.string());
//    debugLog(parser);
//    return parser ? parser(image) : GenericImageParser(image);
//}
//
//ImageParsingFunction ImageParser::_get(const std::filesystem::path& format)
//{
//    auto parser = _getParsers()[format];
//    return parser ? parser->_parsingFunction : nullptr;
//}
