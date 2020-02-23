/*
* @Author: gpi
* @Date:   2019-04-04 13:53:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:47:50
*/

#include "TextureParser.hpp" // for TextureParser, TextureParsingFun...
#include "Debug.hpp" // for debugLog
#include "Texture.hpp" // for Texture
#include "glm/glm.hpp" // for glm::vec2
#include "parser/InternalTools.hpp" // for fileFormat
#include <GL/glew.h> // for GL_COMPRESSED_RGB, GL_COMPRESSED...
#include <SDL2/SDL_error.h> // for SDL_GetError
#include <SDL2/SDL_image.h> // for IMG_Load
#include <SDL2/SDL_pixels.h> // for SDL_PixelFormat, SDL_PIXELFORMAT...
#include <SDL2/SDL_stdinc.h> // for Uint8, Uint16
#include <SDL2/SDL_surface.h> // for SDL_Surface, SDL_UnlockSurface
#include <string.h> // for memcpy
#include <map> // for map
#include <memory> // for allocator, shared_ptr
#include <sstream> // for basic_ostream::operator<<, ostri...
#include <stdexcept> // for runtime_error
#include <stdlib.h> // for free, malloc, NULL
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

std::shared_ptr<Texture> GenericTextureParser(const std::string& name, const std::string& path)
{
    auto surface = IMG_Load(path.c_str());
    if (!surface || !surface->format)
        throw std::runtime_error(std::string("Error parsing ") + path + " : " + SDL_GetError());
    auto textureFormat = surface->format->Amask ? GL_RGBA : GL_RGB;
    auto textureInternalFormat = surface->format->Amask ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB;

    debugLog("Texture Format before conversion :");
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

    debugLog("Texture Format after conversion :");
    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));

    auto texture = Texture::Create(name, glm::vec2(surface->w, surface->h), GL_TEXTURE_2D,
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

std::shared_ptr<Texture> TextureParser::parse(const std::string& name, const std::string& path)
{
    auto format = path.substr(path.find_last_of(".") + 1);
    debugLog(path);
    debugLog(format);
    auto parser = _get(fileFormat(path));
    debugLog(parser);
    auto texture(parser ? parser(name, path) : GenericTextureParser(name, path));
    return texture;
}

TextureParsingFunction TextureParser::_get(const std::string& format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}