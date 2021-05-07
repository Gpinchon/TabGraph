/*
* @Author: gpinchon
* @Date:   2021-05-04 20:08:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:08:59
*/

#include "Texture/TextureBuffer.hpp"
#include "Driver/OpenGL/Texture/Texture.hpp"

class TextureBuffer::Impl : public Texture::Impl {
public:
    Impl(TextureBuffer&);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;
};