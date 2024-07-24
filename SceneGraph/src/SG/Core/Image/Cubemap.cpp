#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Image/Image.hpp>

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

namespace TabGraph::SG {
glm::vec3 CubeMapUVToXYZ(const CubemapSide& a_Side, glm::vec2 uv)
{
    glm::vec3 xyz = glm::vec3(0);
    // convert range 0 to 1 to -1 to 1
    uv = uv * 2.f - 1.f;
    switch (a_Side) {
    case CubemapSide::PositiveX:
        xyz = glm::vec3(1.0f, -uv.y, -uv.x);
        break; // POSITIVE X
    case CubemapSide::NegativeX:
        xyz = glm::vec3(-1.0f, -uv.y, uv.x);
        break; // NEGATIVE X
    case CubemapSide::PositiveY:
        xyz = glm::vec3(uv.x, 1.0f, uv.y);
        break; // POSITIVE Y
    case CubemapSide::NegativeY:
        xyz = glm::vec3(uv.x, -1.0f, -uv.y);
        break; // NEGATIVE Y
    case CubemapSide::PositiveZ:
        xyz = glm::vec3(uv.x, -uv.y, 1.0f);
        break; // POSITIVE Z
    case CubemapSide::NegativeZ:
        xyz = glm::vec3(-uv.x, -uv.y, -1.0f);
        break; // NEGATIVE Z
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

glm::vec3 GetImageUV(const glm::vec3 v)
{
    glm::vec3 vAbs = abs(v);
    float ma;
    glm::vec2 uv;
    float faceIndex;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y) {
        faceIndex = v.z < 0.0 ? 5.0 : 4.0;
        ma        = 0.5 / vAbs.z;
        uv        = glm::vec2(v.z < 0.0 ? -v.x : v.x, -v.y);
    } else if (vAbs.y >= vAbs.x) {
        faceIndex = v.y < 0.0 ? 3.0 : 2.0;
        ma        = 0.5 / vAbs.y;
        uv        = glm::vec2(v.x, v.y < 0.0 ? -v.z : v.z);
    } else {
        faceIndex = v.x < 0.0 ? 1.0 : 0.0;
        ma        = 0.5 / vAbs.x;
        uv        = glm::vec2(v.x < 0.0 ? v.z : -v.z, -v.y);
    }
    return { uv * ma + 0.5f, faceIndex };
}

Cubemap::Cubemap(
    const std::shared_ptr<Image>& a_EquirectangularImage,
    const glm::ivec2& a_Size,
    const Pixel::Description& a_PixelDesc)
{
    SetSize(a_Size);
    SetPixelDesc(a_PixelDesc);
    for (uint side = 0; side < size(); ++side) {
        auto& image = at(side);
        image       = std::make_shared<Image>(ImageType::Image2D, GetPixelDesc(), glm::ivec3(GetSize(), 1u));
        for (auto x = 0; x < image->GetSize().x; ++x) {
            for (auto y = 0; y < image->GetSize().y; ++y) {
                float nx      = std::clamp((float)x / ((float)image->GetSize().x - 0.5f), 0.f, 1.f);
                float ny      = std::clamp((float)y / ((float)image->GetSize().y - 0.5f), 0.f, 1.f);
                auto xyz      = CubeMapUVToXYZ(CubemapSide(side), glm::vec2(nx, ny));
                auto uv       = glm::vec3(XYZToEquirectangular(xyz), 0);
                auto sampleUV = uv * glm::vec3(image->GetSize());
                auto color { a_EquirectangularImage->GetColor(sampleUV, ImageFilter::Bilinear) };
                image->SetColor(glm::vec3(x, y, 0), color);
            }
        }
    }
}

Pixel::Color Cubemap::GetColor(
    const glm::vec3& a_Coords,
    const ImageFilter& a_Filter)
{
    const auto imageUV = GetImageUV(a_Coords);
    return at(int(imageUV.z))->GetColor({ imageUV.x, imageUV.y, 0 }, a_Filter);
}

void Cubemap::SetColor(
    const glm::vec3& a_Coords,
    const Pixel::Color& a_Color)
{
    const auto imageUV = GetImageUV(a_Coords);
    return at(int(imageUV.z))->SetColor({ imageUV.x, imageUV.y, 0 }, a_Color);
}
}