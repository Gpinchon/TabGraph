#include "Texture/Texture2D.hpp"
#include "Config.hpp"
#include "Debug.hpp" // for glCheckError, debugLog
#include "Framebuffer.hpp"
#include "Mesh/Geometry.hpp" // for Geometry
#include "Parser/GLSL.hpp" // for GLSL
#include "Render.hpp" // for DisplayQuad
#include "Shader/Shader.hpp" // for Shader
#include "Tools/Tools.hpp"
#include <cstring> // for memcpy
#include <glm/gtx/rotate_vector.hpp>

Texture2D::Texture2D(const std::string& iname, glm::vec2 s, GLenum f,
    GLenum fi, GLenum data_format, void* data)
    : Texture(iname, GL_TEXTURE_2D)
{
    _format = f;
    _internal_format = fi;
    _data_format = data_format;
    _data_size = get_data_size(data_format);
    _bpp = get_bpp(f, data_format);
    _size = s;
    //_data = static_cast<GLubyte*>(data);
    if (data != nullptr) {
        debugLog(iname);
        debugLog(_size.x);
        debugLog(_size.y);
        debugLog(int(_bpp));

        uint64_t dataTotalSize = _size.x * _size.y * (_bpp / 8);
        debugLog(dataTotalSize);
        _data.resize(dataTotalSize);
        std::memcpy(_data.data(), data, dataTotalSize);
    }
    set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, Config::Get("Anisotropy", 16.f));
    if (values_per_pixel() < 4) {
        set_parameteri(GL_TEXTURE_SWIZZLE_A, GL_ONE);
    }
}

void Texture2D::unload()
{
    if (!_loaded) {
        return;
    }
    glDeleteTextures(1, &_glid);
    _glid = 0u;
    _loaded = false;
}

void Texture2D::load()
{
    if (_loaded)
        return;
    /*auto maxTexRes = Config::Get("MaxTexRes", -1);
    if (maxTexRes > 0 && _data && (_size.x > maxTexRes || _size.y > maxTexRes))
    {
        Resize(glm::ivec2(
            std::min(_size.x, maxTexRes),
            std::min(_size.y, maxTexRes)));
    }*/
    Texture::load();
    glBindTexture(_target, _glid);
    glObjectLabel(GL_TEXTURE, _glid, -1, Name().c_str());
    glBindTexture(_target, 0);
    if (_size.x > 0 && _size.y > 0) {
        glBindTexture(_target, _glid);
        glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format,
            _data_format, _data.data());
        glBindTexture(_target, 0);
    }
    restore_parameters();
    generate_mipmap();
    _loaded = true;
}

glm::ivec2 Texture2D::Size() const
{
    return _size;
}

std::byte* Texture2D::texelfetch(const glm::ivec2& uv)
{
    if (_data.empty()) {
        return (nullptr);
    }
    auto nuv = glm::vec2(
        glm::clamp(int(uv.x), 0, int(_size.x - 1)),
        glm::clamp(int(uv.y), 0, int(_size.y - 1)));
    auto opp = _bpp / 8;
    return data() + (int(_size.x * nuv.y + nuv.x) * opp);
}


void Texture2D::set_pixel(const glm::vec2& uv, const glm::vec4 value)
{
    int opp;
    auto& data = _data;

    opp = _bpp / 8;
    if (data.empty()) {
        data.resize(int(_size.x * _size.y) * opp);
    }
    auto p = texelfetch(uv * glm::vec2(_size));
    auto valuePtr = reinterpret_cast<const float*>(&value);
    for (auto i = 0, j = 0; i < int(opp / _data_size) && j < 4; ++i, ++j) {
        if (_data_size == 1)
            p[i] = std::byte(valuePtr[j] * 255.f);
        else if (_data_size == sizeof(GLfloat))
            static_cast<GLfloat*>((void*)p)[i] = valuePtr[j];
    }
}

void Texture2D::set_pixel(const glm::vec2& uv, const GLubyte* value)
{
    int opp;
    auto& data = _data;

    opp = _bpp / 8;
    if (data.empty()) {
        data.resize(int(_size.x * _size.y) * opp);
    }
    auto p = texelfetch(uv * glm::vec2(_size));
    for (auto i = 0; i < opp; ++i) {
        p[i] = std::byte(value[i]);
    }
}

std::byte* Texture2D::data()
{
    return _data.data();
}

glm::vec4 Texture2D::sample(const glm::vec2& uv)
{
    glm::vec3 vt[4];
    glm::vec4 value { 0, 0, 0, 1 };
    auto& data = _data;

    if (data.empty()) {
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
        auto d = (GLubyte*)&data[int(vt[i].y * _size.x + vt[i].x) * opp];
        for (auto j = 0; j < int(opp / _data_size); ++j) {
            if (_data_size == 1)
                reinterpret_cast<float*>(&value)[j] += (d[j] * vt[i].z) / 255.f;
            else if (_data_size == sizeof(GLfloat))
                reinterpret_cast<float*>(&value)[j] += static_cast<float*>((void*)d)[j] * vt[i].z;
        }
    }
    return (value);
}

void Texture2D::Resize(const glm::ivec2& ns)
{
    if (Size() == ns)
        return;
    auto& data = _data;
    std::vector<std::byte> d;
    if (!data.empty()) {
        auto opp = _bpp / 8;
        d.resize(unsigned(ns.x * ns.y * opp));
        for (auto y = 0; y < ns.y; ++y) {
            for (auto x = 0; x < ns.x; ++x) {
                auto uv = glm::vec2(x / ns.x, y / ns.y);
                auto value = sample(uv);
                auto p = reinterpret_cast<GLubyte*>(&d[int(ns.x * y + x) * opp]);
                for (auto z = 0; z < int(opp / _data_size); ++z) {
                    if (_data_size == 1)
                        p[z] = reinterpret_cast<float*>(&value)[z] * 255.f;
                    else if (_data_size == sizeof(GLfloat))
                        reinterpret_cast<float*>(p)[z] = reinterpret_cast<float*>(&value)[z];
                }
            }
        }
        data = d;
    }
    _size = ns;
    _loaded = false;
    /*if (_glid != 0u)
    {
        glDeleteTextures(1, &_glid);
        glGenTextures(1, &_glid);
        glBindTexture(_target, _glid);
        glObjectLabel(GL_TEXTURE, _glid, -1, Name().c_str());
        glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format,
                     _data_format, _data);
        glBindTexture(_target, 0);
        if (glCheckError(Name())) throw std::runtime_error("");
        restore_parameters();
        _loaded = true;
    }*/
}

std::shared_ptr<Framebuffer>
Texture2D::_generate_blur_buffer(const std::string& bname)
{
    auto buffer = Component::Create<Framebuffer>(bname, Size(), 0, 0);
    buffer->Create_attachement(_format, _internal_format);
    //buffer->setup_attachements();
    return (buffer);
}

void Texture2D::blur(const int& pass, const float& radius, std::shared_ptr<Shader> blurShader)
{
    if (pass == 0)
        return;
    if (_blur_buffer0 == nullptr)
        _blur_buffer0 = _generate_blur_buffer(Name() + "_blur0");
    if (_blur_buffer1 == nullptr)
        _blur_buffer1 = _generate_blur_buffer(Name() + "_blur1");
    _blur_buffer0->Resize(Size());
    _blur_buffer1->Resize(Size());

    static std::shared_ptr<Shader> defaultBlurShader;
    if (defaultBlurShader == nullptr) {
        auto blurVertexCode =
#include "passthrough.vert"
            ;
        auto blurFragmentCode =
#include "blur.frag"
            ;
        defaultBlurShader = Component::Create<Shader>("blur");
        defaultBlurShader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(blurVertexCode, "PassThrough();")));
        defaultBlurShader->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(blurFragmentCode, "Blur();")));
    }
    if (blurShader == nullptr)
        blurShader = defaultBlurShader;

    auto totalPass = pass * 4;
    auto cbuffer = _blur_buffer0;
    auto ctexture = std::static_pointer_cast<Texture2D>(shared_from_this());
    float angle = 0;
    std::shared_ptr<Texture2D> attachement;
    while (totalPass > 0) {
        glm::vec2 direction;
        direction = glm::rotate(glm::vec2(1), angle);
        direction = direction * radius;
        if (totalPass == 1) {
            attachement = cbuffer->attachement(0);
            cbuffer->set_attachement(0, std::static_pointer_cast<Texture2D>(shared_from_this()));
        }
        cbuffer->bind();
        blurShader->SetUniform("in_Direction", direction);
        blurShader->SetTexture("in_Texture_Color", ctexture);
        blurShader->use();
        Render::DisplayQuad()->Draw();
        blurShader->use(false);
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
    Framebuffer::bind_default();
}
