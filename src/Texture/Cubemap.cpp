/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:47:03
*/

#include "Config.hpp"
#include "Texture/Texture2D.hpp" // for Texture2D
#include "Texture/TextureParser.hpp"
#include "Texture/Cubemap.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Tools.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <bits/exception.h>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <thread>

std::vector<std::shared_ptr<Cubemap>> Cubemap::_cubemaps;

Cubemap::Cubemap(const std::string& name)
    : Texture(name)
{
    _target = GL_TEXTURE_CUBE_MAP;
}

Cubemap::~Cubemap()
{
    debugLog(Name());
    unload();
}

std::shared_ptr<Cubemap> Cubemap::Create(const std::string& name)
{
    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(name));
    cubemap->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cubemap->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    cubemap->set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, Config::Get("Anisotropy", 16.f));
    Cubemap::Add(cubemap);
    return (cubemap);
}

/*void Texture::assign(Texture &dest_texture, GLenum target)
{
    if (!_loaded)
        load();
    glBindTexture(_target, _glid);
    glBindTexture(dest_texture._target, dest_texture._glid);
    glTexImage2D(target, 0, dest_texture._internal_format, dest_texture._size.x,
                 dest_texture._size.y, 0, dest_texture._format,
                 dest_texture._data_format, dest_texture._data);
    glBindTexture(_target, 0);
    glBindTexture(dest_texture._target, 0);
}*/

void Cubemap::load()
{
    if (_loaded) {
        return;
    }
    debugLog(Name());
    Texture::load();
    glBindTexture(_target, _glid);
    for (auto i = 0u; i < 6; i++) {
        auto t = side(i);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, t->InternalFormat(),
                    t->Size().x, t->Size().y, 0, t->format(),
                    t->data_format(), t->data());
        glCheckError();
    }
    glBindTexture(_target, 0);
    generate_mipmap();
    restore_parameters();
}

void Cubemap::unload()
{
    if (!_loaded) {
        return;
    }
    for (auto i = 0; i < 6; i++)
        side(i)->unload();
    Texture::unload();
    _loaded = false;
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

float faceTransform[6][2] = {
    { 0, 0 },
    { 0, M_PI },
    { M_PI, 0 },
    { -M_PI / 2.f, 0 },
    { 0, -M_PI / 2.f },
    { 0, M_PI / 2.f }
};

void generate_side(std::shared_ptr<Texture2D> fromTexture, std::shared_ptr<Texture2D> t, int side)
{
    const float ftu = faceTransform[side][1];
    const float ftv = faceTransform[side][0];
    static const float an = sin(M_PI / 4.f);
    static const float ak = cos(M_PI / 4.f);

    for (auto x = 0; x <= t->Size().x; ++x) {
        for (auto y = 0; y <= t->Size().y; ++y) {
            float nx = (float)y / (float)t->Size().x - 0.5f;
            float ny = 1 - (float)x / (float)t->Size().y - 0.5f;
            nx *= 2;
            ny *= 2;
            nx *= an;
            ny *= an;
            float u, v;
            if (ftv == 0) {
                u = atan2(nx, ak);
                v = atan2(ny * cos(u), ak);
                u += ftu;
            } else if (ftv > 0) {
                float d = sqrt(nx * nx + ny * ny);
                v = M_PI / 2.f - atan2(d, ak);
                u = atan2(ny, nx);
            } else {
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
            auto nuv = glm::vec2(y, x) / glm::vec2(t->Size());
            auto val = fromTexture->sample(glm::vec2(u, v));
            t->set_pixel(glm::vec2(nuv.x, nuv.y), val);
        }
    }
    std::cout << "." << std::flush;
}

std::shared_ptr<Cubemap> Cubemap::Create(const std::string& name, std::shared_ptr<Texture2D> fromTexture)
{
    std::cout << "Converting " << fromTexture->Name() << " into Cubemap";
    auto cubemap = Cubemap::Create(name);
    GLenum formats[2];
    fromTexture->format(&formats[0], &formats[1]);
    std::vector<std::thread> threads;
    for (auto i = 0; i < 6; ++i) {
        auto side_res = fromTexture->Size().x / 4.f;
        auto sideTexture = Texture2D::Create(fromTexture->Name() + "_side_" + std::to_string(i), glm::vec2(side_res, side_res), GL_TEXTURE_2D, formats[0], formats[1], fromTexture->data_format());
        threads.push_back(std::thread(generate_side, fromTexture, sideTexture, i));
        cubemap->set_side(i, sideTexture);
    }
    for (auto &thread : threads)
        thread.join();
    cubemap->generate_mipmap();
    cubemap->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    std::cout << " Done." << std::endl;
    debugLog(cubemap.use_count());
    return (cubemap);
}

#include <Config.hpp>

void cubemap_load_side(std::shared_ptr<Cubemap> cubemap, const std::string& path, GLenum iside)
{
    auto sideTexture = TextureParser::parse(path, path);
    if (sideTexture == nullptr) {
        return;
    }
    cubemap->set_side(iside - GL_TEXTURE_CUBE_MAP_POSITIVE_X, sideTexture);
}

std::shared_ptr<Cubemap> Cubemap::parse(const std::string& name, const std::string& path)
{
    try {
        Config cubemapInfo;
        cubemapInfo.Parse(path + name + "/cubemap.info");
        auto t = Cubemap::Create(name);
        cubemap_load_side(t, path + name + "/" + cubemapInfo.Get("POSITIVE_X", std::string("X+.bmp")), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        cubemap_load_side(t, path + name + "/" + cubemapInfo.Get("NEGATIVE_X", std::string("X-.bmp")), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        cubemap_load_side(t, path + name + "/" + cubemapInfo.Get("POSITIVE_Y", std::string("Y+.bmp")), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        cubemap_load_side(t, path + name + "/" + cubemapInfo.Get("NEGATIVE_Y", std::string("Y-.bmp")), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        cubemap_load_side(t, path + name + "/" + cubemapInfo.Get("POSITIVE_Z", std::string("Z+.bmp")), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        cubemap_load_side(t, path + name + "/" + cubemapInfo.Get("NEGATIVE_Z", std::string("Z-.bmp")), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
        t->generate_mipmap();
        t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        debugLog(t.use_count());
        return (t);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing Cubemap : " + path + name + " :\n\t") + e.what());
    }
    return (nullptr);
}

void Cubemap::Add(std::shared_ptr<Cubemap> cubemap)
{
    _cubemaps.push_back(cubemap);
    debugLog(cubemap.use_count());
}

std::shared_ptr<Texture2D> Cubemap::side(unsigned index)
{
    return (_sides.at(index));
}

void Cubemap::set_side(unsigned index, std::shared_ptr<Texture2D> t)
{
    _sides.at(index) = t;
}
