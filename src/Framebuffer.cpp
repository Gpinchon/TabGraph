/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 18:18:37
*/

#include "Framebuffer.hpp"
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <stdexcept>           // for runtime_error
#include "Debug.hpp"           // for glCheckError
#include "Window.hpp"          // for Window

std::vector<std::shared_ptr<Framebuffer>> Framebuffer::_framebuffers;

Attachement::Attachement(const std::string& name)
    : Texture(name)
{
}
Attachement::Attachement(const std::string& name, glm::vec2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format)
    : Texture(name, s, target, f, fi, data_format){};

GLenum get_data_format(GLenum internal_format)
{
    switch (internal_format) {
    case GL_R8_SNORM:
    case GL_RG8_SNORM:
    case GL_RGB8_SNORM:
    case GL_RGBA8_SNORM:
    case GL_SRGB8:
        return (GL_BYTE);
    case GL_R16F:
    case GL_RG16F:
    case GL_RGB16F:
    case GL_RGBA16F:
        return (GL_HALF_FLOAT);
    case GL_R32F:
    case GL_RG32F:
    case GL_RGB32F:
    case GL_RGBA32F:
        return (GL_FLOAT);
    case GL_R11F_G11F_B10F:
        return (GL_UNSIGNED_INT_10F_11F_11F_REV);
    case GL_R16:
    case GL_RG16:
    case GL_RGB16:
        return (GL_UNSIGNED_SHORT);
    case GL_R16_SNORM:
    case GL_RG16_SNORM:
    case GL_RGB16_SNORM:
        return (GL_SHORT);
    default:
        return (GL_UNSIGNED_BYTE);
    }
}

std::shared_ptr<Attachement> Attachement::create(const std::string& iname, glm::vec2 s, GLenum target, GLenum f, GLenum fi)
{
    auto t = std::shared_ptr<Attachement>(new Attachement(iname, s, target, f, fi, get_data_format(fi)));
    glGenTextures(1, &t->_glid);
    glBindTexture(t->_target, t->_glid);
    glObjectLabel(GL_TEXTURE, t->_glid, -1, t->name().c_str());
    glTexImage2D(t->_target, 0, fi, s.x, s.y, 0, f, t->data_format(), nullptr);
    glBindTexture(t->_target, 0);
    glCheckError();
    t->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    t->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _textures.push_back(std::static_pointer_cast<Texture>(t));
    return (t);
}

bool Attachement::is_loaded()
{
    return (true);
}

void Attachement::load()
{
}

void Attachement::unload()
{
}

Framebuffer::Framebuffer(const std::string& name)
    : Texture(name)
{
}

std::shared_ptr<Framebuffer> Framebuffer::create(const std::string& name, glm::vec2 size, int color_attachements, int depth)
{
    int i;

    auto f = std::shared_ptr<Framebuffer>(new Framebuffer(name));
    f->_size = size;
    f->_target = GL_FRAMEBUFFER;
    glGenFramebuffers(1, &f->_glid);
    glBindFramebuffer(f->_target, f->_glid);
    glObjectLabel(f->_target, f->_glid, -1, f->name().c_str());
    glBindFramebuffer(f->_target, 0);
    glCheckError();
    i = 0;
    while (i < color_attachements) {
        f->create_attachement(GL_RGBA, GL_RGBA);
        i++;
    }
    if (depth != 0) {
        f->create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    }
    f->setup_attachements();
    _framebuffers.push_back(f);
    _textures.push_back(std::static_pointer_cast<Texture>(f));
    return (f);
}

std::shared_ptr<Framebuffer> Framebuffer::Get(unsigned index)
{
    if (index >= _framebuffers.size())
        return (nullptr);
    return (_framebuffers.at(index));
}

std::shared_ptr<Framebuffer> Framebuffer::get_by_name(const std::string& name)
{
    for (auto f : _framebuffers) {
        if (name == f->name())
            return (f);
    }
    return (nullptr);
}

bool Framebuffer::is_loaded()
{
    return (true);
}

void Framebuffer::load()
{
    return;
}

void Framebuffer::bind(bool to_bind)
{
    if (!to_bind) {
        bind_default();
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, _glid);
    glViewport(0, 0, size().x, size().y);
    glCheckError();
}

void Framebuffer::bind_default()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::size().x, Window::size().y);
    glCheckError();
}

std::shared_ptr<Texture> Framebuffer::create_attachement(GLenum format, GLenum iformat)
{
    std::string tname;
    if (format == GL_DEPTH_COMPONENT)
        tname = (name() + "_depth");
    else
        tname = (name() + "_attachement_" + std::to_string(_color_attachements.size()));
    bind();
    auto a = Attachement::create(tname, size(), GL_TEXTURE_2D, format, iformat);
    if (format == GL_DEPTH_COMPONENT) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, a->glid(), 0);
        _depth = a;
    } else {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _color_attachements.size(), a->glid(), 0);
        _color_attachements.push_back(a);
    }
    glCheckError();
    bind(false);
    return (a);
}

void Framebuffer::setup_attachements()
{
    unsigned i;
    GLenum format[2];
    std::vector<GLenum> color_attachements;

    i = 0;
    while (i < _color_attachements.size()) {
        attachement(i)->format(&format[0], &format[1]);
        if (format[0] != GL_DEPTH_COMPONENT) {
            color_attachements.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        i++;
    }
    bind();
    glDrawBuffers(color_attachements.size(), &color_attachements[0]);
    glCheckError();
    bind(false);
}

void Framebuffer::_resize_attachement(const int& attachement, const glm::vec2& ns)
{
    auto t = Framebuffer::attachement(attachement);
    if (t == nullptr) {
        return;
    }
    t->resize(ns);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachement, t->glid(), 0);
    glCheckError();
}

void Framebuffer::_resize_depth(const glm::vec2& ns)
{
    if (_depth == nullptr) {
        return;
    }
    _depth->resize(ns);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth->glid(), 0);
    glCheckError();
}

void Framebuffer::resize(const glm::ivec2& new_size)
{
    unsigned i;

    if (size().x == new_size.x && size().y == new_size.y) {
        return;
    }
    bind();
    _size = new_size;
    i = 0;
    while (i < _color_attachements.size()) {
        _resize_attachement(i, new_size);
        i++;
    }
    _resize_depth(new_size);
    bind(false);
}

void Framebuffer::set_attachement(unsigned color_attachement, std::shared_ptr<Texture> texture)
{
    if (color_attachement >= _color_attachements.size())
        throw std::runtime_error(name() + " : Color attachement index is out of bound");
    _color_attachements[color_attachement] = texture;
    bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_attachement, texture->glid(), 0);
    glCheckError();
    bind(false);
}

std::shared_ptr<Texture> Framebuffer::attachement(unsigned color_attachement)
{
    if (unsigned(color_attachement) >= _color_attachements.size())
        return (nullptr);
    return (_color_attachements[color_attachement]);
}

std::shared_ptr<Texture> Framebuffer::depth()
{
    return (_depth);
}
