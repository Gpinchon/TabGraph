/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 19:25:56
*/

#include "Texture/Cubemap.hpp"
#include "Texture/Image.hpp"

#include "Config.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Tools/Tools.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
//#include <bits/exception.h>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <thread>

Cubemap::Cubemap(glm::ivec2 size, Pixel::SizedFormat format)
    : Texture2D(size, format)
{
    _SetType(Texture::Type::TextureCubemap);
    SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Cubemap::Cubemap(std::shared_ptr<Image> fromImage)
    : Texture2D(fromImage)
{
    _SetType(Texture::Type::TextureCubemap);
    SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Cubemap::~Cubemap()
{
    Unload();
}

void Cubemap::Load()
{
    if (GetLoaded())
        return;
    auto image{ GetComponent<Image>() };
    if (image != nullptr)
    {
        if (!image->GetLoaded())
            image->Load();
        SetSize(glm::ivec2(std::min(image->GetSize().x, image->GetSize().y)));
        SetPixelDescription(image->GetPixelDescription());
        if (GetAutoMipMap())
            SetMipMapNbr(MIPMAPNBR(image->GetSize()));
         _Allocate();
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            auto side = Component::Create<Image>(GetSize(), GetPixelDescription());
            ExtractSide(image, side, (Cubemap::Side)sideIndex);
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
        RemoveComponent<Image>(image);
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

void Cubemap::ExtractSide(std::shared_ptr<Image> fromImage, std::shared_ptr<Image> toImage, Side side)
{
    const float an = sin(M_PI / 4.f);
    const float ak = cos(M_PI / 4.f);
    const float faceTransform[6][2] = {
        { 0, 0 },
        { 0, M_PI },
        { M_PI, 0 },
        { -M_PI / 2.f, 0 },
        { 0, -M_PI / 2.f },
        { 0, M_PI / 2.f }
    };
    const float ftu = faceTransform[(int)side][1];
    const float ftv = faceTransform[(int)side][0];

    for (auto x = 0; x < toImage->GetSize().x; ++x) {
        for (auto y = 0; y < toImage->GetSize().y; ++y) {
            float nx = (float)y / (float)toImage->GetSize().x - 0.5f;
            float ny = 1 - (float)x / (float)toImage->GetSize().y - 0.5f;
            nx *= 2;
            ny *= 2;
            nx *= an;
            ny *= an;
            float u, v;
            if (ftv == 0) {
                u = atan2(nx, ak);
                v = atan2(ny * cos(u), ak);
                u += ftu;
            }
            else if (ftv > 0) {
                float d = sqrt(nx * nx + ny * ny);
                v = M_PI / 2.f - atan2(d, ak);
                u = atan2(ny, nx);
            }
            else {
                float d = sqrt(nx * nx + ny * ny);
                v = -M_PI / 2.f + atan2(d, ak);
                u = atan2(-ny, nx);
            }
            u = u / (M_PI);
            v = v / (M_PI / 2.f);
            while (v < -1) {
                v += 2;
                u += 1;
            }
            while (v > 1) {
                v -= 2;
                u += 1;
            }
            while (u < -1) {
                u += 2;
            }
            while (u > 1) {
                u -= 2;
            }
            u = u / 2.0f + 0.5f;
            v = v / 2.0f + 0.5f;
            /*glm::ivec2 suv = glm::vec2(u, v) * glm::vec2(fromImage->GetSize());
            suv = glm::clamp(suv, glm::ivec2(0), fromImage->GetSize() - 1);
            auto nuv = glm::vec2(y, x) / glm::vec2(toImage->GetSize());
            nuv *= toImage->GetSize();
            auto val = fromImage->GetColor(suv);
            toImage->SetColor(nuv, val);*/

            glm::ivec2 sampleTexCoord { glm::vec2(u, v) * glm::vec2(fromImage->GetSize()) };
            sampleTexCoord = glm::clamp(sampleTexCoord, glm::ivec2(0), fromImage->GetSize() - 1);
            auto color{ fromImage->GetColor(sampleTexCoord) };
            toImage->SetColor(glm::vec2(y, x), color);
            //toImage->SetColor(glm::ivec2(x, y), fromImage->GetColor(glm::ivec2(x, y)));
        }
    }
    std::cout << "." << std::flush;
    toImage->SetLoaded(true);;
}

//Cubemap::Cubemap(const std::string& name)
//    : Texture(GL_TEXTURE_CUBE_MAP)
//{
//    SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    SetParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, Config::Get("Anisotropy", 16.f));
//}
//
//#include <future>
//
//Cubemap::Cubemap(const std::string& name, std::shared_ptr<Texture2D> fromTexture)
//    : Cubemap(name)
//{
//    std::cout << "Converting " << fromTexture->GetName() << " into Cubemap";
//    //GLenum formats[2];
//    //fromTexture->format(&formats[0], &formats[1]);
//    std::vector<std::future<void>> threads;
//    for (auto i = 0; i < 6; ++i) {
//        auto side_res = fromTexture->GetSize().x / 4.f;
//        auto sideTexture = Component::Create<Texture2D>(fromTexture->GetName() + "_side_" + std::to_string(i), glm::vec2(side_res, side_res), formats[0], formats[1], fromTexture->data_format());
//        threads.push_back(std::async(generate_side, fromTexture, sideTexture, i));
//        set_side(i, sideTexture);
//    }
//    for (auto& thread : threads)
//        thread.get();
//    GenerateMipmap();
//    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    std::cout << " Done." << std::endl;
//}
//
//Cubemap::~Cubemap()
//{
//    debugLog(Name());
//    unload();
//}
//
///*void Texture::assign(Texture &dest_texture, GLenum target)
//{
//    if (!_loaded)
//        load();
//    glBindTexture(_target, _glid);
//    glBindTexture(dest_texture._target, dest_texture._glid);
//    glTexImage2D(target, 0, dest_texture._internal_format, dest_texture._size.x,
//                 dest_texture._size.y, 0, dest_texture._format,
//                 dest_texture._data_format, dest_texture._data);
//    glBindTexture(_target, 0);
//    glBindTexture(dest_texture._target, 0);
//}*/
//
//void Cubemap::Load()
//{
//    if (GetLoaded()) {
//        return;
//    }
//    debugLog(GetName());
//    GLuint id;
//    glCreateTextures((GLenum)GetType(), 1, &id);
//    SetGlid(id);
//    glBindTexture(_target, _glid);
//    for (auto i = 0u; i < 6; i++) {
//        auto t = side(i);
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, t->InternalFormat(),
//            t->Size().x, t->Size().y, 0, t->format(),
//            t->data_format(), t->data());
//    }
//    glBindTexture(_target, 0);
//    GenerateMipmap();
//    restore_parameters();
//}
//
//void Cubemap::Unload()
//{
//    if (!_loaded) {
//        return;
//    }
//    for (auto i = 0; i < 6; i++)
//        side(i)->unload();
//    Texture::unload();
//    _loaded = false;
//}
//
//#include <Config.hpp>
//
//void cubemap_load_side(std::shared_ptr<Cubemap> cubemap, const std::filesystem::path path, GLenum iside)
//{
//    auto sideTexture = TextureParser::parse(path.string(), path.string());
//    if (sideTexture == nullptr) {
//        return;
//    }
//    cubemap->set_side(iside - GL_TEXTURE_CUBE_MAP_POSITIVE_X, sideTexture);
//}
//
//std::shared_ptr<Cubemap> Cubemap::parse(const std::filesystem::path path)
//{
//    try {
//        auto name = path.filename();
//        Config cubemapInfo;
//        cubemapInfo.Parse(path / "cubemap.info");
//        auto t = Component::Create<Cubemap>(name.string());
//        cubemap_load_side(t, path / cubemapInfo.Get("POSITIVE_X", std::string("X+.bmp")), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
//        cubemap_load_side(t, path / cubemapInfo.Get("NEGATIVE_X", std::string("X-.bmp")), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
//        cubemap_load_side(t, path / cubemapInfo.Get("POSITIVE_Y", std::string("Y+.bmp")), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
//        cubemap_load_side(t, path / cubemapInfo.Get("NEGATIVE_Y", std::string("Y-.bmp")), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
//        cubemap_load_side(t, path / cubemapInfo.Get("POSITIVE_Z", std::string("Z+.bmp")), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
//        cubemap_load_side(t, path / cubemapInfo.Get("NEGATIVE_Z", std::string("Z-.bmp")), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
//        t->generate_mipmap();
//        t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        debugLog(t.use_count());
//        return (t);
//    } catch (std::exception& e) {
//        throw std::runtime_error(std::string("Error parsing Cubemap : " + path.string() + " :\n\t") + e.what());
//    }
//    return (nullptr);
//}
//
//std::shared_ptr<Texture2D> Cubemap::side(unsigned index)
//{
//    return (_sides.at(index));
//}
//
//void Cubemap::set_side(unsigned index, std::shared_ptr<Texture2D> t)
//{
//    _sides.at(index) = t;
//}
