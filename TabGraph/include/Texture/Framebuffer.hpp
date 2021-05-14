/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-11 13:50:12
*/

#pragma once

#include "Texture/PixelUtils.hpp"
#include "Texture/TextureSampler.hpp"

#include <glm/glm.hpp> // for glm::vec2
#include <memory> // for shared_ptr

class Texture;

enum class BufferMask {
    ColorBits = 0x1,
    DepthBits = 0x2,
    StencilBits = 0x3
};

static inline auto operator|(BufferMask a, BufferMask b)
{
    return static_cast<BufferMask>(static_cast<int>(a) | static_cast<int>(b));
}

static inline auto operator&(BufferMask a, BufferMask b)
{
    return static_cast<BufferMask>(static_cast<int>(a) & static_cast<int>(b));
}

class Framebuffer {
public:
    class Impl;
    Framebuffer() = delete;
    /**
    * @brief Creates a framebuffer
    * @param size : base resolution of the framebuffer
    */
    Framebuffer(glm::ivec2 size);
    ~Framebuffer();

    /** @return the specified color attachement */
    std::shared_ptr<Texture> GetColorBuffer(unsigned color_attachement);
    /** @return the depth buffer */
    std::shared_ptr<Texture> GetDepthBuffer();
    /** @return the stencil buffer */
    std::shared_ptr<Texture> GetStencilBuffer();
    /** @brief Adds a color attachement at the end of the attachements list and returns index */
    void AddColorBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel = 0);
    /** @brief Sets the Texture to the specified index */
    void SetColorBuffer(std::shared_ptr<Texture> buffer, unsigned color_attachement = 0, unsigned mipLevel = 0);
    /**
     * @brief Sets the stencil buffer
     * @param buffer : the buffer to be used as stencil buffer
     * @param mipLevel : the mip level to write to
    */
    void SetStencilBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel = 0);
    /** @brief Set the depth buffer */
    void SetDepthBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel = 0);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        glm::ivec2 src0, glm::ivec2 src1,
        glm::ivec2 dst0, glm::ivec2 dst1,
        BufferMask mask = BufferMask::ColorBits | BufferMask::DepthBits | BufferMask::StencilBits,
        TextureSampler::Filter filter = TextureSampler::Filter::Nearest);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        BufferMask mask = BufferMask::ColorBits | BufferMask::DepthBits | BufferMask::StencilBits,
        TextureSampler::Filter filter = TextureSampler::Filter::Nearest);
    glm::ivec2 GetSize() const;
    void SetSize(const glm::ivec2& new_size);

    Impl& GetImpl();

private:
    std::unique_ptr<Impl> _impl;
};
