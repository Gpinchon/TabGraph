/*
* @Author: gpi
* @Date:   2019-04-04 13:53:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:57:27
*/

#include "Texture/Texture2D.hpp" // for Texture2D
#include "Texture/TextureParser.hpp" // for TextureParser, TextureParsingFun...
#include "Debug.hpp" // for debugLog
#include <GL/glew.h> // for GL_COMPRESSED_RGB, GL_COMPRESSED...
#include <glm/glm.hpp> // for glm::vec2
#include <SDL_error.h> // for SDL_GetError
#include <SDL_image.h> // for IMG_Load
#include <SDL_pixels.h> // for SDL_PixelFormat, SDL_PIXELFORMAT...
#include <SDL_stdinc.h> // for Uint8, Uint16
#include <SDL_surface.h> // for SDL_Surface, SDL_UnlockSurface
#include <string.h> // for memcpy
#include <map> // for map
#include <memory> // for allocator, shared_ptr
#include <sstream> // for basic_ostream::operator<<, ostri...
#include <stdexcept> // for runtime_error
#include <stdlib.h> // for free, malloc, NULL
#include <string> // for string, operator+, char_traits
#include <wchar.h> // for memmove
#include <filesystem>

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

std::shared_ptr<Texture2D> GenericTextureParser(const std::string& name, const std::string& path)
{
    auto rwOps = SDL_RWFromFile(path.c_str(), "rb");
    auto surface = IMG_Load_RW(rwOps, 1);
    if (!surface || !surface->format)
        throw std::runtime_error(std::string("Error parsing ") + path + " : " + SDL_GetError());
    auto textureFormat = surface->format->Amask ? GL_RGBA : GL_RGB;
    auto textureInternalFormat = surface->format->Amask ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB;

    debugLog("Texture2D Format before conversion :");
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

    debugLog("Texture2D Format after conversion :");
    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));

    auto texture = Component::Create<Texture2D>(name, glm::vec2(surface->w, surface->h),
        textureFormat, textureInternalFormat, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);
    return (texture);
}

std::map<std::string, TextureParser*>* TextureParser::_parsers = nullptr; //std::map<std::string, TextureParser *>();

TextureParser::TextureParser(const std::string& format, TextureParsingFunction parsingFunction)
    : _format(format)
    , _parsingFunction(parsingFunction)
{
    debugLog(format);
}

TextureParser* TextureParser::Add(const std::string& format, TextureParsingFunction parsingFunction)
{
    auto parser = new TextureParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::map<std::string, TextureParser*>& TextureParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<std::string, TextureParser*>;
    return *_parsers;
}

std::shared_ptr<Texture2D> TextureParser::parse(const std::string& name, const std::string& path)
{
    auto format = std::filesystem::path(path).extension();
    debugLog(path);
    debugLog(format);
    auto parser = _get(format.string());
    debugLog(parser);
    return parser ? parser(name, path) : GenericTextureParser(name, path);
}

TextureParsingFunction TextureParser::_get(const std::string& format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}
