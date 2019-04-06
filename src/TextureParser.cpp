#include "GLIncludes.hpp"
#include "Texture.hpp"
#include "TextureParser.hpp"
#include <SDL2/SDL_image.h>

#include "Debug.hpp"

std::map<std::string, TextureParser *> *TextureParser::_parsers = nullptr;//std::map<std::string, TextureParser *>();

TextureParser::TextureParser(const std::string &format, ParsingFunction parsingFunction)
{
    debugLog(format);
	_parsingFunction = parsingFunction;
    if (_parsers == nullptr){
        _parsers = new std::map<std::string, TextureParser *>;
    }
	(*_parsers)[format] = this;
}

#define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
#define SDL_UNLOCKIFMUST(s) { if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s); }

int invert_surface_vertical(SDL_Surface *surface)
{
    Uint8 *t;
    Uint8 *a, *b;
    Uint8 *last;
    Uint16 pitch;

    if( SDL_LOCKIFMUST(surface) < 0 )
        return -2;

    /* do nothing unless at least two lines */
    if(surface->h < 2) {
        SDL_UNLOCKIFMUST(surface);
        return 0;
    }

    /* get a place to store a line */
    pitch = surface->pitch;
    t = (Uint8*)malloc(pitch);

    if(t == NULL) {
        SDL_UNLOCKIFMUST(surface);
        return -2;
    }

    /* get first line; it's about to be trampled */
    memcpy(t,surface->pixels,pitch);

    /* now, shuffle the rest so it's almost correct */
    a = (Uint8*)surface->pixels;
    last = a + pitch * (surface->h - 1);
    b = last;

    while(a < b) {
        memcpy(a,b,pitch);
        a += pitch;
        memcpy(b,a,pitch);
        b -= pitch;
    }

    /* in this shuffled state, the bottom slice is too far down */
    memmove( b, b+pitch, last-b );

    /* now we can put back that first row--in the last place */
    memcpy(last,t,pitch);

    /* everything is in the right place; close up. */
    free(t);
    SDL_UNLOCKIFMUST(surface);

    return 0;
}

#include <sstream>

std::string hexToString(int hex)
{
    std::ostringstream stream;
    stream << std::hex << hex;
    return stream.str();
}

std::shared_ptr<Texture> GenericTextureParser(const std::string& name, const std::string& path)
{
	auto surface = IMG_Load(path.c_str());
    if(!surface || !surface->format)
        throw std::runtime_error(std::string("Error parsing ") + path + " : " + SDL_GetError());
    invert_surface_vertical(surface);
    GLenum  textureFormat = 0;
    GLenum  textureInternalFormat = 0;

    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));


    /*switch (surface->format->format)
    {
        //case SDL_PIXELFORMAT_UNKNOWN:
        case SDL_PIXELFORMAT_INDEX1LSB:
        case SDL_PIXELFORMAT_INDEX1MSB:
        case SDL_PIXELFORMAT_INDEX4LSB:
        case SDL_PIXELFORMAT_INDEX4MSB:
        case SDL_PIXELFORMAT_INDEX8:
            textureFormat = GL_COLOR_INDEX;
            break;
        case SDL_PIXELFORMAT_RGB332:
        case SDL_PIXELFORMAT_RGB444:
        case SDL_PIXELFORMAT_RGB555:
        case SDL_PIXELFORMAT_RGB565:
        case SDL_PIXELFORMAT_RGB24:
        case SDL_PIXELFORMAT_RGB888:
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            textureFormat = GL_RGB;
            break;
        case SDL_PIXELFORMAT_BGR555:
        case SDL_PIXELFORMAT_BGR565:
        case SDL_PIXELFORMAT_BGR888:
        case SDL_PIXELFORMAT_BGR24:
            textureFormat = GL_BGR;
            break;
        case SDL_PIXELFORMAT_RGBA4444:
        case SDL_PIXELFORMAT_RGBA5551:
        case SDL_PIXELFORMAT_RGBA32:
            textureFormat = GL_RGBA;
            break;
        case SDL_PIXELFORMAT_BGRA4444:
        case SDL_PIXELFORMAT_BGRA5551:
        case SDL_PIXELFORMAT_BGRA8888:
        case SDL_PIXELFORMAT_BGRA32:
            textureFormat = GL_BGRA;
            break;
        //case SDL_PIXELFORMAT_ARGB4444:
        //case SDL_PIXELFORMAT_ARGB1555:
        //case SDL_PIXELFORMAT_ARGB8888:
        //case SDL_PIXELFORMAT_ARGB2101010:
        //case SDL_PIXELFORMAT_ABGR32:
        //    textureFormat = GL_ARGB;
        //    break;
        //case SDL_PIXELFORMAT_ABGR4444:
        //case SDL_PIXELFORMAT_ABGR1555:
        //    textureFormat = GL_ABGR;
        //    break;
        //case SDL_PIXELFORMAT_BGRX8888:
        //case SDL_PIXELFORMAT_ABGR8888:
        //    textureFormat = GL_ABGR;
        //    break;
    }*/

    auto newSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    surface = newSurface;

    textureFormat = GL_RGBA;
    textureInternalFormat = GL_COMPRESSED_RGBA;
    auto texture = Texture::create(name, new_vec2(surface->w, surface->h), GL_TEXTURE_2D,
    textureFormat, textureInternalFormat, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);
    return (texture);
}

std::shared_ptr<Texture> TextureParser::parse(const std::string& name, const std::string& path)
{
	auto format = path.substr(path.find_last_of(".") + 1);
    debugLog(path);
    debugLog(format);
	auto parser = (*_parsers)[format];
    debugLog(parser);
	if (parser == nullptr)
		return (GenericTextureParser(name, path));
	return (parser->_parsingFunction(name, path));
}