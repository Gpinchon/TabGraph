/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-04 20:36:58
*/

#pragma once

#include "Texture.hpp" // for Texture
#include "glm/glm.hpp" // for glm::vec2
#include <GL/glew.h> // for GLenum
#include <memory> // for shared_ptr
#include <string> // for string
#include <vector> // for vector

class Framebuffer : public Texture {
public:
    /**
    * @brief Creates a framebuffer
    * @argument name : name of the framebuffer
    * @argument size : base resolution of the framebuffer
    * @argument color_attachements : number of color attachements to be created
    * @argument depth : set to 1 to enable depth buffer
    */
    static std::shared_ptr<Framebuffer> create(const std::string& name, glm::ivec2 size, int color_attachements, int depth);
    static std::shared_ptr<Framebuffer> get_by_name(const std::string& name);
    static std::shared_ptr<Framebuffer> Get(unsigned index);
    static void bind_default();
    bool is_loaded() override;
    void load() override;
    void bind(bool to_bind = true);
    /** @return the specified color attachement */
    std::shared_ptr<Texture> attachement(unsigned color_attachement);
    /** @return the depth buffer */
    std::shared_ptr<Texture> depth();
    void setup_attachements();
    /** @brief Adds a new attachement to the current buffer and returns it */
    std::shared_ptr<Texture> create_attachement(GLenum format, GLenum iformat);
    void resize(const glm::ivec2& new_size) override;
    /** @brief Sets the texture to the specified index */
    void set_attachement(unsigned color_attachement, std::shared_ptr<Texture>, unsigned mipLevel = 0);
    /** @brief Adds a color attachement at the end of the attachements list and returns index */
    size_t AddAttachement(std::shared_ptr<Texture>);
    /** @brief Set the depth buffer */
    void SetDepthBuffer(std::shared_ptr<Texture>, unsigned mipLevel = 0);

private:
    static std::vector<std::shared_ptr<Framebuffer>> _framebuffers;
    Framebuffer(const std::string& name);
    void _resize_depth(const glm::vec2&);
    void _resize_attachement(const int&, const glm::vec2&);
    void resize_attachement(const int&, const glm::vec2&);
    std::vector<std::shared_ptr<Texture>> _color_attachements;
    std::shared_ptr<Texture> _depth;
};

/*
** Framebuffer Attachements are always loaded by default and cannot be loaded into GPU
*/
class Attachement : public Texture {
public:
    static std::shared_ptr<Attachement> create(const std::string& name, glm::vec2 s, GLenum target, GLenum f, GLenum fi);
    bool is_loaded() override;
    void load() override;
    void unload() override;

private:
    Attachement(const std::string& name);
    Attachement(const std::string& name, glm::vec2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format);
};
