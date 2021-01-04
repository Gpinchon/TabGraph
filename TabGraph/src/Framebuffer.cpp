/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 20:16:52
*/

#include "Framebuffer.hpp"
#include "Debug.hpp" // for glCheckError
#include "Window.hpp" // for Window

#include <stdexcept> // for runtime_error

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

Framebuffer::Framebuffer(const std::string& name, glm::ivec2 size, int color_attachements, int depth)
    : Component(name)
{
    int i;

    _size = size;
    i = 0;
    while (i < color_attachements) {
        Create_attachement(GL_RGBA, GL_RGBA);
        i++;
    }
    if (depth != 0) {
        Create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    }
}

glm::ivec2 Framebuffer::CurrentSize()
{
    glm::ivec4 resolution;
    glGetIntegerv(GL_VIEWPORT, &resolution[0]);
	return glm::ivec2(resolution.z, resolution.w);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_glid);
}

void Framebuffer::bind(bool to_bind)
{
    if (!to_bind) {
        bind_default();
        return;
    }
    LoadGPU();
    setup_attachements();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _glid);
    glViewport(0, 0, Size().x, Size().y);
}

void Framebuffer::bind_default()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::size().x, Window::size().y);
}

std::shared_ptr<Texture2D> Framebuffer::Create_attachement(GLenum format, GLenum iformat)
{
    std::string tname;
    if (format == GL_DEPTH_COMPONENT)
        tname = (Name() + "_depth");
    else
        tname = (Name() + "_attachement_" + std::to_string(_color_attachements.size()));
    auto a = Component::Create<Texture2D>(tname, Size(), format, iformat);
    a->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    a->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (format == GL_DEPTH_COMPONENT) {
        _depth = std::pair(a, 0);
    } else {
        _color_attachements.push_back(std::pair(a, 0));
    }
    _attachementsChanged = true;
    return (a);
}

void Framebuffer::BlitTo(std::shared_ptr<Framebuffer> to, glm::ivec2 src0, glm::ivec2 src1, glm::ivec2 dst0, glm::ivec2 dst1, GLbitfield mask, GLenum filter)
{
    LoadGPU();
    setup_attachements();
    //uint32_t toGLID = 0;
    if (to != nullptr) {
        to->LoadGPU();
        to->setup_attachements();
        to->bind();
        //toGLID = to->_glid;
    }
    else Framebuffer::bind_default();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _glid);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, toGLID);
    glBlitFramebuffer(
        src0.x,
        src0.y,
        src1.x,
        src1.y,
        dst0.x,
        dst0.y,
        dst1.x,
        dst1.y,
        mask,
        filter);
}

void Framebuffer::BlitTo(std::shared_ptr<Framebuffer> to, GLbitfield mask, GLenum filter)
{
    if (to == nullptr)
        BlitTo(to, glm::ivec2(0), Size(), glm::ivec2(0), Window::size(), mask, filter);
    else
        BlitTo(to, glm::ivec2(0), Size(), glm::ivec2(0), to->Size(), mask, filter);
}

void Framebuffer::_LoadGPU()
{
    if (_glid != 0)
        glDeleteFramebuffers(1, &_glid);
    glGenFramebuffers(1, &_glid);
    glBindFramebuffer(GL_FRAMEBUFFER, _glid);
    glObjectLabel(GL_FRAMEBUFFER, _glid, Name().length(), Name().c_str());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    setup_attachements();
    SetLoadedGPU(true);
}

void Framebuffer::setup_attachements()
{
    //if (!_attachementsChanged)
    //    return;
    GLenum format[2];
    std::vector<GLenum> color_attachements;
    for (auto attachement : _color_attachements)
        attachement.first->load();
    if (_depth.first != nullptr)
        _depth.first->load();

    glBindFramebuffer(GL_FRAMEBUFFER, _glid);
    for (auto i = 0u; i < _color_attachements.size(); ++i) {
        auto attachement(_color_attachements.at(i));
        attachement.first->format(&format[0], &format[1]);
        if (format[0] != GL_DEPTH_COMPONENT) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, attachement.first->glid(), attachement.second);
            color_attachements.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
    }
    if (_depth.first != nullptr) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth.first->glid(), _depth.second);
    }
    if (!color_attachements.empty())
        glDrawBuffers(color_attachements.size(), &color_attachements.at(0));
    else
        glDrawBuffers(0, nullptr);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _attachementsChanged = false;
}
/*
void Framebuffer::_resize_attachement(const int& attachement, const glm::vec2& ns)
{
    auto t = Framebuffer::attachement(attachement);
    if (t == nullptr) {
        return;
    }
    t->Resize(ns);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachement, t->glid(), 0);
    //if (glCheckError(Name()))
        throw std::runtime_error("");
}

void Framebuffer::_resize_depth(const glm::vec2& ns)
{
    if (_depth.first == nullptr) {
        return;
    }
    _depth.first->Resize(ns);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth->glid(), 0);
    //if (glCheckError(Name()))
        throw std::runtime_error("");
}
*/

glm::ivec2 Framebuffer::Size() const
{
    return _size;
}

void Framebuffer::Resize(const glm::ivec2& new_size)
{
    if (Size() == new_size) {
        return;
    }
    _size = new_size;
    for (auto attachement : _color_attachements)
        attachement.first->Resize(new_size);
    if (_depth.first != nullptr)
        _depth.first->Resize(new_size);
    _attachementsChanged = true;
}

size_t Framebuffer::AddAttachement(std::shared_ptr<Texture2D> texture2D)
{
    _color_attachements.push_back(std::pair(texture2D, 0));
    set_attachement(_color_attachements.size() - 1, texture2D);
    return _color_attachements.size() - 1;
}

void Framebuffer::set_attachement(unsigned color_attachement, std::shared_ptr<Texture2D> texture2D, unsigned mipLevel)
{
    try {
        _color_attachements.at(color_attachement).first = texture2D;
        _color_attachements.at(color_attachement).second = mipLevel;
    }
    catch (std::runtime_error &e) {throw std::runtime_error(Name() + " : " + e.what()); }
    _attachementsChanged = true;
    /*bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_attachement, texture2D ? texture2D->glid() : 0, mipLevel);
    if (glCheckError(Name()))
        throw std::runtime_error("");
    bind(false);*/
}

void Framebuffer::SetDepthBuffer(std::shared_ptr<Texture2D> depth, unsigned mipLevel)
{
    _depth.first = depth;
    _depth.second = mipLevel;
    _attachementsChanged = true;
    /*bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth ? depth->glid() : 0, mipLevel);
    if (glCheckError(Name()))
        throw std::runtime_error("");
    bind(false);*/
}

std::shared_ptr<Texture2D> Framebuffer::attachement(unsigned color_attachement)
{
    return _color_attachements.at(color_attachement).first;
}

std::shared_ptr<Texture2D> Framebuffer::depth()
{
    return _depth.first;
}
