/*
* @Author: gpinchon
* @Date:   2019-04-04 13:53:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:05
*/

#include "Texture/ImageParser.hpp" // for ImageParser, TextureParsingFun...
#include "Debug.hpp" // for debugLog
#include "Texture/Image.hpp" // for Image
#include <GL/glew.h> // for GL_COMPRESSED_RGB, GL_COMPRESSED...
#include <SDL_error.h> // for SDL_GetError
#include <SDL_image.h> // for IMG_Load
#include <SDL_pixels.h> // for SDL_PixelFormat, SDL_PIXELFORMAT...
#include <SDL_stdinc.h> // for Uint8, Uint16
#include <SDL_surface.h> // for SDL_Surface, SDL_UnlockSurface
#include <filesystem>
#include <glm/glm.hpp> // for glm::vec2
#include <map> // for map
#include <memory> // for allocator, shared_ptr
#include <sstream> // for basic_ostream::operator<<, ostri...
#include <stdexcept> // for runtime_error
#include <stdlib.h> // for free, malloc, NULL
#include <string.h> // for memcpy
#include <string> // for string, operator+, char_traits
#include <wchar.h> // for memmove

#define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
#define SDL_UNLOCKIFMUST(s)       \
    {                             \
        if (SDL_MUSTLOCK(s))      \
            SDL_UnlockSurface(s); \
    }

int invert_surface_vertical(SDL_Surface* surface)
{
    Uint8* t;
    Uint8 *a, *b;
    Uint8* last;
    Uint16 pitch;

    if (SDL_LOCKIFMUST(surface) < 0)
        return -2;
    if (surface->h < 2) {
        SDL_UNLOCKIFMUST(surface);
        return 0;
    }
    pitch = surface->pitch;
    t = (Uint8*)malloc(pitch);
    if (t == NULL) {
        SDL_UNLOCKIFMUST(surface);
        return -2;
    }
    memcpy(t, surface->pixels, pitch);
    a = (Uint8*)surface->pixels;
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

void GenericImageParser(const std::shared_ptr<Image> &image)
{
    auto rwOps = SDL_RWFromFile(image->GetPath().string().c_str(), "rb");
    auto surface = IMG_Load_RW(rwOps, 1);
    if (!surface || !surface->format)
        throw std::runtime_error(std::string("Error parsing ") + image->GetPath().string() + " : " + SDL_GetError());

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

    //image->SetPixelFormat(pixelFormat);
    auto pixelFormat = surface->format->Amask ? Pixel::SizedFormat::Uint8_NormalizedRGBA : Pixel::SizedFormat::Uint8_NormalizedRGB;
    image->SetPixelDescription(pixelFormat);
    image->SetSize(glm::vec2(surface->w, surface->h));
    auto bufferSize{ surface->h * surface->pitch };
    image->SetData(std::vector<std::byte>((std::byte*)surface->pixels, (std::byte*)surface->pixels + bufferSize));
    //auto ima = Component::Create<Image>(glm::vec2(surface->w, surface->h),
    //    textureFormat, textureInternalFormat, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);
    image->SetLoaded(true);
}

std::map<std::filesystem::path, ImageParser*>* ImageParser::_parsers = nullptr; //std::map<std::string, ImageParser *>();

ImageParser::ImageParser(const std::filesystem::path& format, ImageParsingFunction parsingFunction)
    : _format(format)
    , _parsingFunction(parsingFunction)
{
    debugLog(format);
}

ImageParser* ImageParser::Add(const std::filesystem::path& format, ImageParsingFunction parsingFunction)
{
    auto parser = new ImageParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::map<std::filesystem::path, ImageParser*>& ImageParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<std::filesystem::path, ImageParser*>;
    return *_parsers;
}

std::shared_ptr<Image> ImageParser::Parse(const std::filesystem::path& path)
{
    auto image = Component::Create<Image>(path);
    Parse(image);
    return image;
}

void ImageParser::Parse(std::shared_ptr<Image> image)
{
    auto format = image->GetPath().extension();
    debugLog(image->GetPath());
    debugLog(format);
    auto parser = _get(format.string());
    debugLog(parser);
    return parser ? parser(image) : GenericImageParser(image);
}

ImageParsingFunction ImageParser::_get(const std::filesystem::path& format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}
