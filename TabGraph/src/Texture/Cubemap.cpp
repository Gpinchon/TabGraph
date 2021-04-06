/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 19:25:56
*/

#include "Texture/Cubemap.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Config.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Tools/Tools.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <thread>

Cubemap::Cubemap(glm::ivec2 size, Pixel::SizedFormat format)
    : Texture2D(size, format)
{
    _SetType(Texture::Type::TextureCubemap);
    SetParameter<Texture::Parameter::MagFilter>(Texture::Filter::Linear);
    SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::Linear);
}

Cubemap::Cubemap(std::shared_ptr<Asset> fromImage)
    : Texture2D(fromImage)
{
    _SetType(Texture::Type::TextureCubemap);
    SetParameter<Texture::Parameter::MagFilter>(Texture::Filter::Linear);
    SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::Linear);
}

Cubemap::~Cubemap()
{
    Unload();
}

void Cubemap::Load()
{
    if (GetLoaded())
        return;
    auto imageAsset{ GetComponent<Asset>() };
    if (imageAsset != nullptr)
    {
        imageAsset->Load();
        auto image{ imageAsset->GetComponent<Image>() };
        assert(image != nullptr);
        SetSize(glm::ivec2(std::min(image->GetSize().x, image->GetSize().y)));
        SetPixelDescription(image->GetPixelDescription());
        if (GetAutoMipMap())
            SetMipMapNbr(MIPMAPNBR(image->GetSize()));
         _Allocate();
         std::array<std::shared_ptr<Image>, 6> sides;
         std::array<std::thread, 6> loadingThreads;
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            sides.at(sideIndex) = Component::Create<Image>(GetSize(), GetPixelDescription());
            loadingThreads.at(sideIndex) = std::thread(ExtractSide, image, sides.at(sideIndex), (Cubemap::Side)sideIndex);
        }
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            loadingThreads.at(sideIndex).join();
            auto side{ sides.at(sideIndex) };
            glTextureSubImage3D(
                GetHandle(),
                0,
                0,
                0,
                sideIndex,
                side->GetSize().x,
                side->GetSize().y,
                1,
                (GLenum)side->GetPixelDescription().GetUnsizedFormat(),
                (GLenum)side->GetPixelDescription().GetType(),
                side->GetData().data()
            );
        }
        RemoveComponent(imageAsset);
        if (GetAutoMipMap())
            GenerateMipmap();
    }
    else
        _Allocate();
    _SetLoaded(true);
    RestoreParameters();
}

void Cubemap::_Allocate()
{
    _SetHandle(Texture::Create(GetType()));
    glTextureStorage2D(
        GetHandle(),
        GetMipMapNbr(),
        (GLenum)GetPixelDescription().GetSizedFormat(),
        GetSize().x, GetSize().y);
}

glm::vec3 outImgToXYZ(float u, float v, int faceIdx)
{
    glm::vec3 xyz;
    auto a = 2.0 * u;
    auto b = 2.0 * v;

    if (faceIdx == 0) // back
        xyz = glm::vec3(-1.0, 1.0 - a, 1.0 - b);
    else if (faceIdx == 1) // left
        xyz = glm::vec3(a - 1.0, -1.0, 1.0 - b);
    else if (faceIdx == 2) // front)
        xyz = glm::vec3(1.0, a - 1.0, 1.0 - b);
    else if (faceIdx == 3) // right)
        xyz = glm::vec3(1.0 - a, 1.0, 1.0 - b);
    else if (faceIdx == 4) // top
        xyz = glm::vec3(b - 1.0, a - 1.0, 1.0);
    else if (faceIdx == 5) // bottom
        xyz = glm::vec3(1.0 - b, a - 1.0, -1.0);
    return (xyz);
}

auto HDRColor(std::shared_ptr<Image> hdr_image,
    float dx, float dy, float dz)
{
    // assume angle map projection
    const float one_over_pi = 1.0f / M_PI;
    float invl = 1.0f / sqrtf(dx * dx + dy * dy);
    float r = one_over_pi * acosf(dz) * invl;
    float u = dx * r; // -1..1
    float v = dy * r; // -1..1
    // map to pixel coordinates
    int x = int(u * hdr_image->GetSize().x + hdr_image->GetSize().x) >> 1;
    int y = int(v * hdr_image->GetSize().y + hdr_image->GetSize().y) >> 1;
    // return the float RGB value at (x,y)
    return hdr_image->GetColor(glm::ivec2(x, y));
}

glm::vec3 CubeMapUVToXYZ(const int index, glm::vec2 uv)
{
    glm::vec3 xyz = glm::vec3(0);
    // convert range 0 to 1 to -1 to 1
    uv = uv * 2.f - 1.f;
    switch (index)
    {
    case 0:
        xyz = glm::vec3(1.0f, -uv.y, -uv.x);
        break;  // POSITIVE X
    case 1:
        xyz = glm::vec3(-1.0f, -uv.y, uv.x);
        break;  // NEGATIVE X
    case 2:
        xyz = glm::vec3(uv.x, 1.0f, uv.y);
        break;  // POSITIVE Y
    case 3:
        xyz = glm::vec3(uv.x, -1.0f, -uv.y);
        break;  // NEGATIVE Y
    case 4:
        xyz = glm::vec3(uv.x, -uv.y, 1.0f);
        break;  // POSITIVE Z
    case 5:
        xyz = glm::vec3(-uv.x, -uv.y, -1.0f);
        break;  // NEGATIVE Z
    }
    return normalize(xyz);
}

const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
glm::vec2 XYZToEquirectangular(glm::vec3 xyz)
{
    glm::vec2 uv = glm::vec2(atan2(xyz.z, xyz.x), asin(xyz.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void Cubemap::ExtractSide(std::shared_ptr<Image> fromImage, std::shared_ptr<Image> toImage, Side side)
{
    for (auto x = 0; x < toImage->GetSize().x; ++x) {
        for (auto y = 0; y < toImage->GetSize().y; ++y) {
            float nx = std::clamp((float)x / ((float)toImage->GetSize().x - 0.5f), 0.f, 1.f);
            float ny = std::clamp((float)y / ((float)toImage->GetSize().y - 0.5f), 0.f, 1.f);
            auto xyz = CubeMapUVToXYZ((int)side, glm::vec2(nx, ny));
            auto uv = XYZToEquirectangular(xyz);
            glm::ivec2 sampleTexCoord { uv * glm::vec2(fromImage->GetSize()) };
            sampleTexCoord = glm::clamp(sampleTexCoord, glm::ivec2(0), fromImage->GetSize() - 1);
            auto color{ fromImage->GetColor(sampleTexCoord) };
            //auto color{ HDRColor(fromImage, xyz.x, xyz.y, xyz.z) };
            toImage->SetColor(glm::vec2(x, y), color);
            //toImage->SetColor(glm::ivec2(x, y), fromImage->GetColor(glm::ivec2(x, y)));
        }
    }
    std::cout << "." << std::flush;
}
