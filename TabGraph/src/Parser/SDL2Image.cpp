/*
* @Author: gpinchon
* @Date:   2019-04-04 13:53:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:05
*/

#include <Assets/Image.hpp>
#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>
#include <Buffer/BufferView.hpp>
#include <Config.hpp>
#include <Texture/PixelUtils.hpp>
#include <Debug.hpp>

#include <memory> // for allocator, shared_ptr
#include <SDL_image.h> // for IMG_Load
#include <SDL_pixels.h> // for SDL_PixelFormat, SDL_PIXELFORMAT...
#include <SDL_rwops.h>             // for SDL_RWops
#include <SDL_surface.h> // for SDL_Surface, SDL_UnlockSurface
#include <sstream>
#include <stdlib.h> // for free, malloc, NULL
#include <string>                 // for string

void SDL2ImageParser(const std::shared_ptr<Asset>&);

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

std::string hexToString(int hex)
{
    std::ostringstream stream;
    stream << std::hex << hex;
    return stream.str();
}

void SDL2ImageParser(const std::shared_ptr<Asset>& container)
{
    auto uri{ container->GetUri() };
    SDL_RWops* rwOps{ nullptr };
    std::vector<std::byte> data;
    if (uri.GetScheme() == "data") {
        data = DataUri(uri).Decode();
        if (data.empty()) {
            auto& bufferView{ container->GetComponent<BufferView>() };
            if (bufferView != nullptr) { //We're loading raw bytes from a BufferView
                bufferView->Load();
                auto dataPtr{ bufferView->Get(0, bufferView->GetByteLength()) };
                data = std::vector<std::byte>(
                    dataPtr, dataPtr + bufferView->GetByteLength()
                    );
            }
        }
        rwOps = SDL_RWFromMem(data.data(), data.size());
    }
    else {
        rwOps = SDL_RWFromFile(uri.DecodePath().u8string().c_str(), "rb");
    }

    auto surface = IMG_Load_RW(rwOps, 1);
    assert(surface != nullptr);
    assert(surface->format != nullptr);

    debugLog("Image Format before conversion :");
    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));

    if (SDL_PIXELFORMAT_RGBA32 != surface->format->format) {
        auto newSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        assert(newSurface != nullptr);
        SDL_FreeSurface(surface);
        surface = newSurface;
    }
    const auto bufferSize{ surface->h * surface->pitch };
    const auto pixelFormat = Pixel::SizedFormat::Uint8_NormalizedRGBA;
    auto image = Component::Create<Image>(glm::ivec2(surface->w, surface->h), pixelFormat, std::vector((std::byte*)surface->pixels, (std::byte*)surface->pixels + bufferSize));
    SDL_FreeSurface(surface);
    surface = nullptr;
    if (container->parsingOptions.image.maximumResolution != -1) {
        auto texRes{ glm::min(image->GetSize(), glm::ivec2(container->parsingOptions.image.maximumResolution)) };
        image->SetSize(texRes, Image::SamplingFilter::Bilinear);
    }
    container->SetComponent(image);
    container->SetAssetType(Image::AssetType);
    container->SetLoaded(true);
}
