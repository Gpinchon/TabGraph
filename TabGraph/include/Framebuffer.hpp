/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:25:53
*/

#pragma once

#include "Texture/Texture2D.hpp" // for Texture2D
#include <glm/glm.hpp> // for glm::vec2
#include <GL/glew.h> // for GLenum
#include <memory> // for shared_ptr
#include <string> // for string
#include <vector> // for vector

class Framebuffer : public Component {
public:
    Framebuffer() = delete;
    Framebuffer(const std::string& name, glm::ivec2 size, int color_attachements, int depth);
    /** @return the currently bound Framebuffer resolution */
    static glm::ivec2 CurrentSize();
    /**
    * @brief Creates a framebuffer
    * @argument name : name of the framebuffer
    * @argument size : base resolution of the framebuffer
    * @argument color_attachements : number of color attachements to be Created
    * @argument depth : set to 1 to enable depth buffer
    */
    static void bind_default();
    void bind(bool to_bind = true);
    /** @return the specified color attachement */
    std::shared_ptr<Texture2D> attachement(unsigned color_attachement);
    /** @return the depth buffer */
    std::shared_ptr<Texture2D> depth();
    
    /** @brief Adds a new attachement to the current buffer and returns it */
    std::shared_ptr<Texture2D> Create_attachement(GLenum format, GLenum iformat);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        glm::ivec2 src0, glm::ivec2 src1,
        glm::ivec2 dst0, glm::ivec2 dst1,
        GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GLenum filter = GL_LINEAR);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GLenum filter = GL_LINEAR);
    virtual glm::ivec2 Size() const;
    virtual void Resize(const glm::ivec2& new_size);
    /** @brief Sets the texture2D to the specified index */
    virtual void set_attachement(unsigned color_attachement, std::shared_ptr<Texture2D>, unsigned mipLevel = 0);
    /** @brief Adds a color attachement at the end of the attachements list and returns index */
    virtual size_t AddAttachement(std::shared_ptr<Texture2D>);
    /** @brief Set the depth buffer */
    virtual void SetDepthBuffer(std::shared_ptr<Texture2D>, unsigned mipLevel = 0);
    virtual ~Framebuffer() override;

private:
    //void _resize_depth(const glm::vec2&);
    //void _resize_attachement(const int&, const glm::vec2&);
    virtual std::shared_ptr<Component> _Clone() override {
        auto framebuffer(Component::Create<Framebuffer>(*this));
        return framebuffer;
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    void resize_attachement(const int&, const glm::vec2&);
    void setup_attachements();
    std::vector<std::pair<std::shared_ptr<Texture2D>, unsigned>> _color_attachements;
    std::pair<std::shared_ptr<Texture2D>, unsigned> _depth;
    glm::ivec2 _size { 0, 0 };
    GLuint _glid { 0 };
    bool _attachementsChanged { true };
};
