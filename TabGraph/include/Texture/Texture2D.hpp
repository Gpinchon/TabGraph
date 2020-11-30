#pragma once

#include "Texture/Texture.hpp"

class Texture2D : public Texture {
public:
    Texture2D(const std::string& name, glm::vec2 s, GLenum f, GLenum fi, GLenum data_format = GL_UNSIGNED_BYTE, void* data = nullptr);
    //Texture2D(const std::string& name, glm::vec2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format = GL_UNSIGNED_BYTE, void* data = nullptr);
    /**@brief Fetches a texel with pixel coordinate and returns it as raw bytes*/
    virtual std::byte* texelfetch(const glm::ivec2& uv);
    virtual std::byte *data();
    virtual glm::vec4 sample(const glm::vec2& uv);
    virtual glm::ivec2 Size() const;
    virtual void Resize(const glm::ivec2& ns);
    virtual void set_pixel(const glm::vec2& uv, const glm::vec4 value);
    virtual void set_pixel(const glm::vec2& uv, const GLubyte* value);
    virtual void blur(const int& pass, const float& radius, std::shared_ptr<Shader> = nullptr);
    virtual void load() override;
    virtual void unload() override;
    template <typename T>
    T* at(float u, float v);

protected:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<Texture2D>(*this);
    }
    /*virtual void _LoadCPU() override;
    virtual void _UnloadCPU() override;
    virtual void _LoadGPU() override;
    virtual void _UnloadGPU() override;*/
    glm::ivec2 _size { 0, 0 };
    std::shared_ptr<Framebuffer> _blur_buffer0;
    std::shared_ptr<Framebuffer> _blur_buffer1;
    std::shared_ptr<Framebuffer> _generate_blur_buffer(const std::string&);
};

template <typename T>
T* Texture2D::at(float u, float v)
{
    auto nuv = glm::vec2(
        glm::clamp(round(_size.x * u), 0.f, float(_size.x - 1)),
        glm::clamp(round(_size.y * v), 0.f, float(_size.y - 1)));
    auto opp = _bpp / 8;
    return (&_data[static_cast<int>(nuv.y * _size.x + nuv.x) * opp * sizeof(T)]);
}