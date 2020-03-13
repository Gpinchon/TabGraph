/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-29 15:43:22
*/

#pragma once

#include "Object.hpp" // for Object
#include "glm/glm.hpp" // for s_vec2, glm::vec2, glm::vec2, CLAMP, glm::vec4, s_vec4
#include <GL/glew.h> // for GLenum, GLubyte, GL_UNSIGNED_BYTE, GLuint
#include <math.h> // for round
#include <memory> // for shared_ptr
#include <stddef.h> // for size_t
#include <string> // for string
#include <unordered_map> // for unordered_map
#include <vector> // for vector

class Shader;
class Framebuffer;

class Texture : public Object
{
public:
    //static std::shared_ptr<Texture> Create(const std::string &name, glm::ivec2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format = GL_UNSIGNED_BYTE, void *data = nullptr);
    static std::shared_ptr<Texture> GetByName(const std::string &);
    static std::shared_ptr<Texture> Get(unsigned index);
    static size_t get_data_size(GLenum data_type);
    static size_t get_bpp(GLenum texture_format, GLenum data_type);
    virtual bool is_loaded();
    virtual void set_parameteri(GLenum p, int v);
    virtual void set_parameterf(GLenum p, float v);
    virtual void restore_parameters();
    virtual void load();
    virtual void unload();
    virtual void generate_mipmap();
    virtual GLenum target() const;
    virtual void format(GLenum *format, GLenum *internal_format);
    virtual GLenum format();
    virtual GLenum internal_format();
    virtual GLenum data_format();
    virtual size_t data_size();
    virtual GLuint glid() const;
    virtual void *data() const;
    virtual GLubyte bpp() const;
    virtual size_t values_per_pixel();

protected:
    Texture(const std::string &name);
    //Texture(const std::string &name, glm::vec2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format = GL_UNSIGNED_BYTE, void *data = nullptr);
    GLuint _glid{0};
    char _bpp{0};
    size_t _data_size{0};
    GLenum _data_format{0};
    GLenum _target{0};
    GLenum _format{0};
    GLenum _internal_format{0};
    GLubyte *_data{nullptr};
    bool _loaded{false};
    bool _mipMapsGenerated{false};
    std::unordered_map<GLenum, int> _parametersi;
    std::unordered_map<GLenum, float> _parametersf;

private:
    //static std::vector<std::shared_ptr<Texture>> _textures;
};
