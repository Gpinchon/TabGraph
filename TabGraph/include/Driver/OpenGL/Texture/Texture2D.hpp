/*
* @Author: gpinchon
* @Date:   2021-05-02 00:29:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-02 20:41:39
*/
#pragma once
#include <Driver/OpenGL/Texture/Texture.hpp>
#include <Texture/Texture2D.hpp>
#include <Event/Signal.hpp>

struct Event;

class Texture2D::Impl : public Texture::Impl {
public:
    Impl(const Impl&) = delete;
    Impl(Texture2D&);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

private:
    void _AllocateStorage();
    void _UploadImage(std::shared_ptr<Asset> imageAsset);
    Signal<const Event&>::ScoppedSlot _loadedSlot;
    bool _allocated{ false };
};