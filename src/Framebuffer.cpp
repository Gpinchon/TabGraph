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

std::vector<std::shared_ptr<Framebuffer>> Framebuffer::_framebuffers;

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

Framebuffer::Framebuffer(const std::string& name)
    : Object(name)
{
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_glid);
}

std::shared_ptr<Framebuffer> Framebuffer::Create(const std::string& name, glm::ivec2 size, int color_attachements, int depth)
{
    int i;

    auto f = std::shared_ptr<Framebuffer>(new Framebuffer(name));
    f->_size = size;
    i = 0;
    while (i < color_attachements) {
        f->Create_attachement(GL_RGBA, GL_RGBA);
        i++;
    }
    if (depth != 0) {
        f->Create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    }
    //f->setup_attachements();
    Framebuffer::Add(f);
    return (f);
}

void Framebuffer::Add(std::shared_ptr<Framebuffer> framebuffer)
{
    //Texture2D::Add(framebuffer);
    _framebuffers.push_back(framebuffer);
}

std::shared_ptr<Framebuffer> Framebuffer::Get(unsigned index)
{
    if (index >= _framebuffers.size())
        return (nullptr);
    return (_framebuffers.at(index));
}

std::shared_ptr<Framebuffer> Framebuffer::GetByName(const std::string& name)
{
    for (auto f : _framebuffers) {
        if (name == f->Name())
            return (f);
    }
    return (nullptr);
}

void Framebuffer::bind(bool to_bind)
{
    if (!to_bind) {
        bind_default();
        return;
    }
    for (auto attachement : _color_attachements)
        attachement.first->load();
    if (_depth.first != nullptr)
        _depth.first->load();
    if (_glid == 0u) {
        glGenFramebuffers(1, &_glid);
        if (glCheckError(Name()))
                throw std::runtime_error("Error while generating Framebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, _glid);
        glObjectLabel(GL_FRAMEBUFFER, _glid, Name().length(), Name().c_str());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (glCheckError(Name()))
                throw std::runtime_error("Error while bind default Framebuffer");
    }
    if (_attachementsChanged) {
        setup_attachements();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, _glid);
    if (glCheckError(Name()))
            throw std::runtime_error("Error while binding Framebuffer");
    glViewport(0, 0, Size().x, Size().y);
    if (glCheckError(Name()))
            throw std::runtime_error("Error while setting viewport");
}

void Framebuffer::bind_default()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (glCheckError())
        throw std::runtime_error("Error while binding default framebuffer");
    glViewport(0, 0, Window::size().x, Window::size().y);
    if (glCheckError())
        throw std::runtime_error("Error while setting viewport");
}

std::shared_ptr<Texture2D> Framebuffer::Create_attachement(GLenum format, GLenum iformat)
{
    std::string tname;
    if (format == GL_DEPTH_COMPONENT)
        tname = (Name() + "_depth");
    else
        tname = (Name() + "_attachement_" + std::to_string(_color_attachements.size()));
    auto a = Texture2D::Create(tname, Size(), GL_TEXTURE_2D, format, iformat);
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

void Framebuffer::setup_attachements()
{
    GLenum format[2];
    std::vector<GLenum> color_attachements;

    glBindFramebuffer(GL_FRAMEBUFFER, _glid);
    if (glCheckError(Name()))
            throw std::runtime_error("Error while binding Framebuffer");
    for (auto i = 0u; i < _color_attachements.size(); ++i) {
        auto attachement(_color_attachements.at(i));
        attachement.first->format(&format[0], &format[1]);
        if (format[0] != GL_DEPTH_COMPONENT) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, attachement.first->glid(), attachement.second);
            if (glCheckError(Name()))
                    throw std::runtime_error("Error while setting Framebuffer texture " + attachement.first->Name());
            color_attachements.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
    }
    if (_depth.first != nullptr) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth.first->glid(), _depth.second);
        if (glCheckError(Name()))
                throw std::runtime_error("Error while setting Framebuffer Depth attachement " + _depth.first->Name());
    }
    glDrawBuffers(color_attachements.size(), &color_attachements[0]);
    if (glCheckError(Name()))
            throw std::runtime_error("Error while setting Framebuffer drawbuffers");
#ifdef DEBUG_MOD
    auto status(glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
            case (GL_FRAMEBUFFER_UNDEFINED) : 
                debugLog(Name() + " GL_FRAMEBUFFER_UNDEFINED");
                break;
            case (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) : 
                debugLog(Name() + " GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
                break;
            case (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ) : 
                debugLog(Name() + " GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ");
                break;
            case (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER ) : 
                debugLog(Name() + " GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER ");
                break;
            case (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER ) : 
                debugLog(Name() + " GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER ");
                break;
            case (GL_FRAMEBUFFER_UNSUPPORTED ) : 
                debugLog(Name() + " GL_FRAMEBUFFER_UNSUPPORTED ");
                break;
            case (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) : 
                debugLog(Name() + " GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
                break;
            case (GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS ) : 
                debugLog(Name() + " GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS ");
                break;
        }
    }
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (glCheckError(Name()))
            throw std::runtime_error("Error while binding default Framebuffer");
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
