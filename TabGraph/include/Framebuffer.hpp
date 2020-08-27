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

class Framebuffer : public Object {
public:
    /**
    * @brief Creates a framebuffer
    * @argument name : name of the framebuffer
    * @argument size : base resolution of the framebuffer
    * @argument color_attachements : number of color attachements to be Created
    * @argument depth : set to 1 to enable depth buffer
    */
    static std::shared_ptr<Framebuffer> Create(const std::string& name, glm::ivec2 size, int color_attachements, int depth);
    static std::shared_ptr<Framebuffer> GetByName(const std::string& name);
    static std::shared_ptr<Framebuffer> Get(unsigned index);
    static void Add(std::shared_ptr<Framebuffer>);
    static void bind_default();
    void bind(bool to_bind = true);
    /** @return the specified color attachement */
    std::shared_ptr<Texture2D> attachement(unsigned color_attachement);
    /** @return the depth buffer */
    std::shared_ptr<Texture2D> depth();
    
    /** @brief Adds a new attachement to the current buffer and returns it */
    std::shared_ptr<Texture2D> Create_attachement(GLenum format, GLenum iformat);
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
    static std::vector<std::shared_ptr<Framebuffer>> _framebuffers;
    Framebuffer() = delete;
    Framebuffer(const std::string& name);
    //void _resize_depth(const glm::vec2&);
    //void _resize_attachement(const int&, const glm::vec2&);
    void resize_attachement(const int&, const glm::vec2&);
    void setup_attachements();
    std::vector<std::pair<std::shared_ptr<Texture2D>, unsigned>> _color_attachements;
    std::pair<std::shared_ptr<Texture2D>, unsigned> _depth;
    glm::ivec2 _size { 0, 0 };
    GLuint _glid { 0 };
    bool _attachementsChanged { true };
};

/*
** Framebuffer Attachements are always loaded by default and cannot be loaded into GPU
*/
/*
class Attachement : public Texture2D {
public:
    static std::shared_ptr<Attachement> Create(const std::string& name, glm::vec2 s, GLenum target, GLenum f, GLenum fi);
    bool is_loaded() override;
    void load() override;
    void unload() override;

private:
    Attachement(const std::string& name, glm::vec2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format);
};
*/
