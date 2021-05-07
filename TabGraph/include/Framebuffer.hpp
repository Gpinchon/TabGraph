/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:43
*/

#pragma once

#include "Component.hpp"
#include "Texture/PixelUtils.hpp"

#include <glm/glm.hpp> // for glm::vec2
#include <memory> // for shared_ptr
#include <string> // for string
#include <vector> // for vector

class Texture;

class Framebuffer : public Component {
    using Handle = GLuint;
    PROPERTY(bool, Loaded, false);
    PROPERTY(Handle, Handle, 0);
public:
    Framebuffer() = delete;
    /**
    * @brief Creates a framebuffer
    * @param name : name of the framebuffer
    * @param size : base resolution of the framebuffer
    * @param color_attachements : number of color attachements to be Created
    * @param depth : set to 1 to enable depth buffer
    */
    Framebuffer(const std::string& name, glm::ivec2 size, int color_attachements = 0, int depth = 0, int multiSample = 0);
    /** @return the currently bound Framebuffer resolution */
    static glm::ivec2 CurrentSize();
    
    static void bind_default();
    void bind(bool to_bind = true);
    /** @return the specified color attachement */
    std::shared_ptr<Texture> GetColorBuffer(unsigned color_attachement);
    /** @return the depth buffer */
    std::shared_ptr<Texture> GetDepthBuffer();
    /** @return the stencil buffer */
    std::shared_ptr<Texture> GetStencilBuffer();

    void BlitTo(std::shared_ptr<Framebuffer> to,
        glm::ivec2 src0, glm::ivec2 src1,
        glm::ivec2 dst0, glm::ivec2 dst1,
        GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GLenum filter = GL_NEAREST);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GLenum filter = GL_NEAREST);
    virtual glm::ivec2 Size() const;
    virtual void Resize(const glm::ivec2& new_size);
    /** @brief Adds a color attachement at the end of the attachements list and returns index */
    virtual Framebuffer &AddColorBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel = 0);
    /** @brief Adds a color attachement at the end of the attachements list and returns index */
    virtual Framebuffer &AddColorBuffer(Pixel::SizedFormat pixelFormat, unsigned mipLevel = 0);
    /** @brief Sets the Texture to the specified index */
    virtual Framebuffer &SetColorBuffer(std::shared_ptr<Texture> buffer, unsigned color_attachement = 0, unsigned mipLevel = 0);
    /**
     * @brief Sets the stencil buffer
     * @param buffer : the buffer to be used as stencil buffer
     * @param mipLevel : the mip level to write to
    */
    virtual Framebuffer &SetStencilBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel = 0);
    /** @brief Set the depth buffer */
    virtual Framebuffer &SetDepthBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel = 0);
    virtual ~Framebuffer() override;
    virtual void Load();

private:
    //void _resize_depth(const glm::vec2&);
    //void _resize_attachement(const int&, const glm::vec2&);
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto framebuffer(Component::Create<Framebuffer>(*this));
        return framebuffer;
    }
    void setup_attachements();
    std::vector<std::pair<std::shared_ptr<Texture>, unsigned>> _colorBuffers;
    std::pair<std::shared_ptr<Texture>, unsigned> _depthBuffer;
    std::pair<std::shared_ptr<Texture>, unsigned> _stencilBuffer;
    glm::ivec2 _size { 0, 0 };
    bool _attachementsChanged { true };
};
