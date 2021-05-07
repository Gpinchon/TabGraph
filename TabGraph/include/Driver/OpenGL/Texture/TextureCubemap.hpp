/*
* @Author: gpinchon
* @Date:   2021-05-04 15:09:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/
#pragma once
#include "Driver/OpenGL/Texture/Texture.hpp"
#include "Texture/TextureCubemap.hpp"

class TextureCubemap::Impl : public Texture::Impl {
public:
    Impl(const Impl&) = delete;
    Impl(TextureCubemap&);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

private:
    void _AllocateStorage();
    void _UploadImage(std::shared_ptr<Asset> imageAsset);
};