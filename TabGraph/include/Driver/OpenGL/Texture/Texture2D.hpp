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
#include <DispatchQueue.hpp>

struct Event;

namespace Pixel {
struct Description;
}

class Texture2D::Impl : public Texture::Impl {
public:
    Impl(const Impl& other);
    Impl(const glm::ivec2& size, const Pixel::Description& pixelDesc);
    Impl(std::shared_ptr<Asset>);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

    void SetSize(const glm::ivec2& size);
    void SetCompressed(bool);
    inline void SetImage(std::shared_ptr<Asset> asset)
    {
        _asset = asset;
    }

    inline glm::ivec2 GetSize() const
    {
        return _size;
    }
    inline bool GetCompressed() const
    {
        return _compressed;
    }
    inline std::shared_ptr<Asset> GetImage() const
    {
        return _asset;
    }

private:
    void _AllocateStorage();
    void _UploadImage(std::shared_ptr<Asset> imageAsset);
    bool _compressed{ false };
    glm::ivec2 _size{ 0 };
    std::shared_ptr<Asset> _asset;
    Signal<const Event&>::ScoppedSlot _imageLoadingSlot;
    Signal<const Event&>::ScoppedSlot _imageCompressionSlot;
    DispatchQueue::TaskIdentifier _imageCompressionTaskID;
    std::vector<std::byte> _imageCompressionBuffer;
};