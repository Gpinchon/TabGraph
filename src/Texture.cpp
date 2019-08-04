/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-04 22:23:26
*/

#include "Texture.hpp"
#include "Config.hpp" // for Config
#include "Debug.hpp" // for glCheckError, debugLog
#include "Engine.hpp" // for M_PI
#include "Framebuffer.hpp" // for Framebuffer
#include "Render.hpp" // for DisplayQuad
#include "Shader.hpp" // for Shader
#include "VertexArray.hpp" // for VertexArray
#include "parser/GLSL.hpp" // for GLSL
#include <Tools.hpp>
#include <algorithm> // for min
#include <cstring> // for memcpy
#include <glm/gtx/rotate_vector.hpp>
#include <stdint.h> // for int16_t, uint64_t
#include <utility> // for pair

std::vector<std::shared_ptr<Texture>> Texture::_textures;

Texture::Texture(const std::string& name)
    : Object(name)
{
}

Texture::Texture(const std::string& iname, glm::vec2 s, GLenum target, GLenum f,
    GLenum fi, GLenum data_format, void* data)
    : Texture(iname)
{
    _target = target;
    _format = f;
    _internal_format = fi;
    _data_format = data_format;
    _data_size = get_data_size(data_format);
    _bpp = get_bpp(f, data_format);
    _size = s;
    //_data = static_cast<GLubyte*>(data);
    if (data != nullptr) {
        uint64_t dataTotalSize = _size.x * _size.y * _bpp / 8;
        _data = new GLubyte[dataTotalSize];
        std::memcpy(_data, data, dataTotalSize);
    }
}

std::shared_ptr<Texture> Texture::create(const std::string& name, glm::ivec2 s,
    GLenum target, GLenum f, GLenum fi,
    GLenum data_format, void* data)
{
    auto t = std::shared_ptr<Texture>(
        new Texture(name, s, target, f, fi, data_format, data));
    t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    t->set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, Config::Get("Anisotropy", 16.f));
    if (t->values_per_pixel() < 4) {
        t->set_parameteri(GL_TEXTURE_SWIZZLE_A, GL_ONE);
    }
    _textures.push_back(t);
    return (t);
}

std::shared_ptr<Texture> Texture::get_by_name(const std::string& name)
{
    for (auto t : _textures) {
        if (name == t->name())
            return (t);
    }
    return (nullptr);
}

std::shared_ptr<Texture> Texture::shared_from_this()
{
    return (std::static_pointer_cast<Texture>(Object::shared_from_this()));
}

size_t Texture::get_bpp(GLenum texture_format, GLenum data_format)
{
    auto data_size = Texture::get_data_size(data_format);
    switch (texture_format) {
    case GL_RED:
        return (1 * 8 * data_size);
    case GL_RG:
        return (2 * 8 * data_size);
    case GL_RGB:
    case GL_BGR:
        return (3 * 8 * data_size);
    case GL_RGBA:
    case GL_BGRA:
        return (4 * 8 * data_size);
    default:
        return (0);
    }
}

size_t Texture::get_data_size(GLenum data_format)
{
    switch (data_format) {
    case GL_FLOAT:
    case GL_FIXED:
    case GL_INT:
    case GL_UNSIGNED_INT:
        return (sizeof(GLfloat));
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return (sizeof(GLubyte));
    case GL_HALF_FLOAT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return (sizeof(GLushort));
    case GL_DOUBLE:
        return (sizeof(GLdouble));
    default:
        return (0);
    }
}

GLenum Texture::target() const { return (_target); }

glm::vec2 Texture::size() const { return (_size); }

void* Texture::data() const { return (_data); }

void Texture::assign(Texture& dest_texture, GLenum target)
{
    if (!_loaded)
        load();
    glBindTexture(_target, _glid);
    glBindTexture(dest_texture._target, dest_texture._glid);
    glTexImage2D(target, 0, dest_texture._internal_format, dest_texture._size.x,
        dest_texture._size.y, 0, dest_texture._format,
        dest_texture._data_format, dest_texture._data);
    glBindTexture(_target, 0);
    glBindTexture(dest_texture._target, 0);
}

void Texture::unload()
{
    if (!_loaded) {
        return;
    }
    glDeleteTextures(1, &_glid);
    _loaded = false;
    _glid = 0u;
}

void Texture::load()
{
    if (_loaded) {
        return;
    }
    auto maxTexRes = Config::Get("MaxTexRes", -1);
    if (maxTexRes > 0 && _data && (_size.x > maxTexRes || _size.y > maxTexRes)) {
        resize(glm::ivec2(
            std::min(_size.x, maxTexRes),
            std::min(_size.y, maxTexRes)));
    }
    if (_glid == 0u) {
        glGenTextures(1, &_glid);
        glBindTexture(_target, _glid);
        glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
        glBindTexture(_target, 0);
    }
    if (_size.x > 0 && _size.y > 0) {
        glBindTexture(_target, _glid);
        glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format,
            _data_format, _data);
        glBindTexture(_target, 0);
    }
    glCheckError();
    restore_parameters();
    generate_mipmap();
    _loaded = true;
}

void Texture::generate_mipmap()
{
    //if (_mipMapsGenerated)
    //    return;
    glBindTexture(_target, _glid);
    glGenerateMipmap(_target);
    glCheckError();
    glBindTexture(_target, 0);
    _mipMapsGenerated = true;
}

void Texture::format(GLenum* format, GLenum* internal_format)
{
    *format = _format;
    *internal_format = _internal_format;
}

GLubyte Texture::bpp() const { return (_bpp); }

GLuint Texture::glid() const { return (_glid); }

GLenum Texture::internal_format() { return (_internal_format); }

GLenum Texture::format() { return (_format); }

GLenum Texture::data_format() { return (_data_format); }

size_t Texture::data_size() { return (_data_size); }

size_t Texture::values_per_pixel()
{
    return (_data_size ? _bpp / _data_size / 8 : 0);
}

GLubyte* Texture::texelfetch(const glm::ivec2& uv)
{
    if (_data == nullptr) {
        return (nullptr);
    }
    auto nuv = glm::vec2(
        glm::clamp(int(uv.x), 0, int(_size.x - 1)),
        glm::clamp(int(uv.y), 0, int(_size.y - 1)));
    auto opp = _bpp / 8;
    return (&_data[int(_size.x * nuv.y + nuv.x) * opp]);
}

void Texture::set_pixel(const glm::vec2& uv, const glm::vec4 value)
{
    int opp;
    glm::vec4 val { 0, 0, 0, 1 };

    opp = _bpp / 8;
    val = value;
    if (_data == nullptr) {
        _data = new GLubyte[int(_size.x * _size.y) * opp];
    }
    GLubyte* p;
    p = texelfetch(uv * glm::vec2(_size));
    auto valuePtr = reinterpret_cast<float*>(&val);
    for (auto i = 0, j = 0; i < int(opp / _data_size) && j < 4; ++i, ++j) {
        if (_data_size == 1)
            p[i] = valuePtr[j] * 255.f;
        else if (_data_size == sizeof(GLfloat))
            static_cast<GLfloat*>((void*)p)[i] = valuePtr[j];
    }
}

void Texture::set_pixel(const glm::vec2& uv, const GLubyte* value)
{
    int opp;

    opp = _bpp / 8;
    if (_data == nullptr) {
        _data = new GLubyte[int(_size.x * _size.y) * opp];
    }
    GLubyte* p;
    p = texelfetch(uv * glm::vec2(_size));
    for (auto i = 0; i < opp; ++i) {
        p[i] = value[i];
    }
}

void Texture::set_parameterf(GLenum p, float v)
{
    _parametersf[p] = v;
    if (_glid == 0u)
        return;
    if (glTextureParameterf == nullptr) {
        glBindTexture(_target, _glid);
        glTexParameterf(_target, p, v);
        glBindTexture(_target, 0);
    } else {
        glTextureParameterf(_glid, p, v);
    }
    glCheckError();
}

void Texture::set_parameteri(GLenum p, int v)
{
    _parametersi[p] = v;
    if (_glid == 0u)
        return;
    if (glTextureParameteri == nullptr) {
        glBindTexture(_target, _glid);
        glTexParameteri(_target, p, v);
        glBindTexture(_target, 0);
    } else {
        glTextureParameteri(_glid, p, v);
    }
    if (glCheckError() != GL_NO_ERROR) {
        debugLog(name());
    }
}

void Texture::restore_parameters()
{
    for (auto p : _parametersi) {
        set_parameteri(p.first, p.second);
    }
    for (auto p : _parametersf) {
        set_parameterf(p.first, p.second);
    }
}

glm::vec4 Texture::sample(const glm::vec2& uv)
{
    glm::vec3 vt[4];
    glm::vec4 value { 0, 0, 0, 1 };

    if (_data == nullptr) {
        return (value);
    }
    vt[0] = glm::vec3(
        glm::clamp(_size.x * uv.x, 0.f, float(_size.x - 1)),
        glm::clamp(_size.y * uv.y, 0.f, float(_size.y - 1)),
        0);
    auto nuv = glm::vec2(glm::fract(vt[0].x), glm::fract(vt[0].y));
    vt[0].x = int(vt[0].x);
    vt[0].y = int(vt[0].y);
    vt[0].z = ((1 - nuv.x) * (1 - nuv.y));
    vt[1] = glm::vec3(std::min(float(_size.x - 1), vt[0].x + 1),
        std::min(float(_size.y - 1), vt[0].y + 1), (nuv.x * (1 - nuv.y)));
    vt[2] = glm::vec3(vt[0].x, vt[1].y, ((1 - nuv.x) * nuv.y));
    vt[3] = glm::vec3(vt[1].x, vt[0].y, (nuv.x * nuv.y));
    auto opp = _bpp / 8;
    for (auto i = 0; i < 4; ++i) {
        auto d = &_data[int(vt[i].y * _size.x + vt[i].x) * opp];
        for (auto j = 0; j < int(opp / _data_size); ++j) {
            if (_data_size == 1)
                reinterpret_cast<float*>(&value)[j] += (d[j] * vt[i].z) / 255.f;
            else if (_data_size == sizeof(GLfloat))
                reinterpret_cast<float*>(&value)[j] += static_cast<float*>((void*)d)[j] * vt[i].z;
        }
    }
    return (value);
}

bool Texture::is_loaded() { return (_loaded); }

void Texture::resize(const glm::ivec2& ns)
{
    GLubyte* d;

    _loaded = false;
    if (_data != nullptr) {
        auto opp = _bpp / 8;
        d = new GLubyte[unsigned(ns.x * ns.y * opp)];
        for (auto y = 0; y < ns.y; ++y) {
            for (auto x = 0; x < ns.x; ++x) {
                auto uv = glm::vec2(x / ns.x, y / ns.y);
                auto value = sample(uv);
                auto p = &d[int(ns.x * y + x) * opp];
                for (auto z = 0; z < int(opp / _data_size); ++z) {
                    if (_data_size == 1)
                        p[z] = reinterpret_cast<float*>(&value)[z] * 255.f;
                    else if (_data_size == sizeof(GLfloat))
                        reinterpret_cast<float*>(p)[z] = reinterpret_cast<float*>(&value)[z];
                }
            }
        }
        delete[] _data;
        _data = d;
    }
    _size = ns;
    if (_glid != 0u) {
        glDeleteTextures(1, &_glid);
        glGenTextures(1, &_glid);
        glBindTexture(_target, _glid);
        glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
        glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format,
            _data_format, _data);
        glBindTexture(_target, 0);
        glCheckError();
        restore_parameters();
    }
}

std::shared_ptr<Framebuffer>
Texture::_generate_blur_buffer(const std::string& bname)
{
    auto buffer = Framebuffer::create(bname, size(), 0, 0);
    buffer->create_attachement(_format, _internal_format);
    buffer->setup_attachements();
    return (buffer);
}

void Texture::blur(const int& pass, const float& radius)
{
    if (pass == 0)
        return;
    if (_blur_buffer0 == nullptr)
        _blur_buffer0 = _generate_blur_buffer(name() + "_blur0");
    if (_blur_buffer1 == nullptr)
        _blur_buffer1 = _generate_blur_buffer(name() + "_blur1");
    _blur_buffer0->resize(size());
    _blur_buffer1->resize(size());

    static auto blurVertexCode =
#include "passthrough.vert"
        ;
    static auto blurFragmentCode =
#include "blur.frag"
        ;
    static auto blurShader = GLSL::compile("blur", blurVertexCode, blurFragmentCode);

    auto totalPass = pass * 4;
    auto cbuffer = _blur_buffer0;
    auto ctexture = shared_from_this();
    float angle = 0;
    std::shared_ptr<Texture> attachement;
    blurShader->use();
    while (totalPass > 0) {
        glm::vec2 direction;
        direction = glm::rotate(glm::vec2(1), angle);
        direction = direction * radius;
        if (totalPass == 1) {
            attachement = cbuffer->attachement(0);
            cbuffer->set_attachement(0, shared_from_this());
        }
        cbuffer->bind();
        blurShader->set_uniform("in_Direction", direction);
        blurShader->bind_texture("in_Texture_Color", ctexture, GL_TEXTURE0);
        Render::DisplayQuad()->draw();
        angle = CYCLE(angle + (M_PI / 4.f), 0, M_PI);
        if (totalPass == 1)
            cbuffer->set_attachement(0, attachement);
        if (totalPass % 2 == 0) {
            cbuffer = _blur_buffer1;
            ctexture = _blur_buffer0->attachement(0);
        } else {
            cbuffer = _blur_buffer0;
            ctexture = _blur_buffer1->attachement(0);
        }
        totalPass--;
    }
    blurShader->use(false);
    Framebuffer::bind_default();
}
