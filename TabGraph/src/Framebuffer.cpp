/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:34
*/

#include "Framebuffer.hpp"
#include "Texture/Texture.hpp" // for Texture
#include "Texture/Texture2D.hpp" // for Texture2D
#include "Texture/Texture2DMultisample.hpp" // for Texture2D
#include "Debug.hpp" // for glCheckError
#include "Window.hpp" // for Window

#include <GL/glew.h> // for GLenum
#include <stdexcept> // for runtime_error

Framebuffer::Framebuffer(const std::string& name, glm::ivec2 size, int color_attachements, int depth, int multisample)
    : Component(name)
{
    int i;

    _size = size;
    i = 0;
    while (i < color_attachements) {
        //TODO Implement Texture2DMultisample
        //if (multiSample > 0)
        //    SetColorBuffer(Component::Create<Texture2DMultisample>(size, Pixel::SizedFormat::Uint8_NormalizedRGBA, multiSample), i);
        //else
            SetColorBuffer(Component::Create<Texture2D>(size, Pixel::SizedFormat::Uint8_NormalizedRGBA), i);
        i++;
    }
    if (depth != 0) {
        //TODO Implement Texture2DMultisample
        //if (multiSample > 0)
        //    SetDepthBuffer(Component::Create<Texture2D>(size, Pixel::SizedFormat::Depth24, multiSample));
        //else
            SetDepthBuffer(Component::Create<Texture2D>(size, Pixel::SizedFormat::Depth24));
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
    auto glid{ GetHandle() };
    glDeleteFramebuffers(1, &glid);
}

#include "Shader/Global.hpp"

void Framebuffer::bind(bool to_bind)
{
    if (!to_bind) {
        bind_default();
        return;
    }
    Load();
    setup_attachements();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetHandle());
    glViewport(0, 0, Size().x, Size().y);
    Shader::Global::SetUniform("Resolution", glm::vec3(Size(), Size().x / float(Size().y)));
}

void Framebuffer::bind_default()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::GetSize().x, Window::GetSize().y);
    Shader::Global::SetUniform("Resolution", glm::vec3(Window::GetSize(), Window::GetSize().x / float(Window::GetSize().y)));
}

void Framebuffer::BlitTo(std::shared_ptr<Framebuffer> to, glm::ivec2 src0, glm::ivec2 src1, glm::ivec2 dst0, glm::ivec2 dst1, GLbitfield mask, GLenum filter)
{
    Load();
    setup_attachements();
    //uint32_t toGLID = 0;
    if (to != nullptr) {
        to->Load();
        to->setup_attachements();
        to->bind();
        //toGLID = to->_glid;
    } else
        Framebuffer::bind_default();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GetHandle());
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
        BlitTo(to, glm::ivec2(0), Size(), glm::ivec2(0), Window::GetSize(), mask, filter);
    else
        BlitTo(to, glm::ivec2(0), Size(), glm::ivec2(0), to->Size(), mask, filter);
}

void Framebuffer::Load()
{
    if (GetLoaded())
        return;
    auto glid{ GetHandle() };
    if (GetHandle() != 0)
        glDeleteFramebuffers(1, &glid);
    glGenFramebuffers(1, &glid);
    SetHandle(glid);
    glBindFramebuffer(GL_FRAMEBUFFER, glid);
    glObjectLabel(GL_FRAMEBUFFER, glid, GetName().length(), GetName().c_str());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    setup_attachements();
    SetLoaded(true);
}

#include "Driver/OpenGL/Texture/Texture.hpp"
#include <Texture/Texture2D.hpp>
#include <Texture/TextureCubemap.hpp>

void Framebuffer::setup_attachements()
{
    if (!_attachementsChanged)
        return;
    std::vector<GLenum> color_attachements;
    for (auto attachement : _colorBuffers) {
        if (attachement.first != nullptr)
            attachement.first->Load();
    }
    //Distinct depth and stencil buffers are not allowed, use Depth24_Stencil8 texture format.
    if (_depthBuffer.first != nullptr && _stencilBuffer.first != nullptr)
        assert(_depthBuffer.first == _stencilBuffer.first);
    if (_depthBuffer.first != nullptr)
        _depthBuffer.first->Load();
    if (_stencilBuffer.first != nullptr)
        _stencilBuffer.first->Load();
    glBindFramebuffer(GL_FRAMEBUFFER, GetHandle());
    if (_depthBuffer.first != nullptr)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthBuffer.first->GetImpl().GetHandle(), _depthBuffer.second);
    else
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
    if (_stencilBuffer.first != nullptr)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, _stencilBuffer.first->GetImpl().GetHandle(), _stencilBuffer.second);
    else
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
    for (auto i = 0u; i < _colorBuffers.size(); ++i) {
        auto attachement(_colorBuffers.at(i));
        if (attachement.first == nullptr) {
            color_attachements.push_back(GL_NONE);
            continue;
        }
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, attachement.first->GetHandle(), attachement.second);
        color_attachements.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(color_attachements.size(), color_attachements.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _attachementsChanged = false;
}

glm::ivec2 Framebuffer::Size() const
{
    return _size;
}

auto ResizeAttachement(std::shared_ptr<Texture> attachement, glm::ivec2 res)
{
    switch (attachement->GetType())
    {
    case (Texture::Type::Texture2D):
        std::static_pointer_cast<Texture2D>(attachement)->SetSize(res);
    case (Texture::Type::TextureCubemap):
        std::static_pointer_cast<TextureCubemap>(attachement)->SetSize(res);
    }
}

void Framebuffer::Resize(const glm::ivec2& new_size)
{
    if (Size() == new_size) {
        return;
    }
    _size = new_size;
    for (auto attachement : _colorBuffers) {
        if (attachement.first != nullptr)
            ResizeAttachement(attachement.first, new_size);
    }
    if (_depthBuffer.first != nullptr)
        ResizeAttachement(_depthBuffer.first, new_size);
    if (_stencilBuffer.first != nullptr)
        ResizeAttachement(_stencilBuffer.first, new_size);
    _attachementsChanged = true;
}

Framebuffer& Framebuffer::AddColorBuffer(std::shared_ptr<Texture> attachement, unsigned mipLevel)
{
    return SetColorBuffer(attachement, _colorBuffers.size());
}

Framebuffer& Framebuffer::AddColorBuffer(Pixel::SizedFormat pixelFormat, unsigned mipLevel)
{
    return AddColorBuffer(Component::Create<Texture2D>(Size(), pixelFormat), mipLevel);
}

Framebuffer& Framebuffer::SetColorBuffer(std::shared_ptr<Texture> attachement, unsigned color_attachement, unsigned mipLevel)
{
    //_colorBuffers.push_back(std::pair(attachement, mipLevel));
    if (_colorBuffers.size() <= color_attachement)
        _colorBuffers.resize(color_attachement + 1);
    try {
        _attachementsChanged |= attachement != _colorBuffers.at(color_attachement).first;
        _attachementsChanged |= mipLevel != _colorBuffers.at(color_attachement).second;
        _colorBuffers.at(color_attachement) = std::make_pair(attachement, mipLevel);
    }
    catch (std::runtime_error& e) {
        throw std::runtime_error(GetName() + " : " + e.what());
    }
    return *this;
}

Framebuffer& Framebuffer::SetStencilBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel)
{
    _attachementsChanged |= buffer != _stencilBuffer.first;
    _attachementsChanged |= mipLevel != _stencilBuffer.second;
    _stencilBuffer = std::make_pair(buffer, mipLevel);
    return *this;
}

Framebuffer& Framebuffer::SetDepthBuffer(std::shared_ptr<Texture> buffer, unsigned mipLevel)
{
    _attachementsChanged |= buffer != _depthBuffer.first;
    _attachementsChanged |= mipLevel != _depthBuffer.second;
    _depthBuffer = std::make_pair(buffer, mipLevel);
    return *this;
}

std::shared_ptr<Texture> Framebuffer::GetColorBuffer(unsigned color_attachement)
{
    return _colorBuffers.at(color_attachement).first;
}

std::shared_ptr<Texture> Framebuffer::GetDepthBuffer()
{
    return _depthBuffer.first;
}

std::shared_ptr<Texture> Framebuffer::GetStencilBuffer()
{
    return _stencilBuffer.first;
}
