/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:09
*/

#pragma once

#include "Object.hpp" // for Object
#include "glm/glm.hpp" // for glm::mat4, glm::vec2, glm::vec3
#include <GL/glew.h> // for GLuint, GLenum, GLint
#include <memory> // for shared_ptr
#include <stddef.h> // for size_t
#include <string> // for string
#include <unordered_map> // for unordered_map
#include <vector> // for vector

class Texture; // lines 16-16

struct ShaderVariable {
    size_t id;
    std::string name;
    GLint size;
    GLenum type;
    GLint loc { -1 };
};

class ShaderStage {
public:
    ShaderStage(GLenum stage = 0, const std::string code = "");
    ~ShaderStage();
    void Compile();
    void Recompile();
    std::string Code() const;
    GLuint Glid() const;
    GLenum Stage() const;
    bool Compiled() const;
    void SetDefine(const std::string, const std::string = "");
    void RemoveDefine(const std::string);
    std::string Technique() const;
    void SetTechnique(const std::string);
    void Delete();

private:
    GLuint _glid {0};
    GLenum _stage {0};
    std::string _code { "" };
    std::string _technique { "" };
    bool _compiled { false };
    std::unordered_map<std::string, std::string> _defines;
};

enum ShaderType {
    Other,
    ForwardShader,
    LightingShader,
    PostShader,
    ComputeShader
};

class Shader : public Object {
public:
    static std::shared_ptr<Shader> Create(const std::string&, ShaderType type = Other);
    static std::shared_ptr<Shader> Get(unsigned index);
    static std::shared_ptr<Shader> GetByName(const std::string&);
    static bool check_shader(const GLuint id);
    static bool check_program(const GLuint id);
    /*void attach(const GLuint shaderid);
    void detach(const GLuint shaderid);
    GLuint link(const GLuint vertexid, const GLuint fragmentid);
    GLuint link(const GLuint geometryid, const GLuint vertexid, const GLuint fragmentid);
    GLuint link(const GLuint shaderid);*/
    void bind_texture(const std::string& uname, std::shared_ptr<Texture>, const GLenum texture_unit);
    void bind_image(const std::string& uname, std::shared_ptr<Texture> texture, const GLint level, const bool layered, const GLint layer, const GLenum access, const GLenum texture_unit);
    void unbind_texture(GLenum texture_unit);
    void SetUniform(const std::string& uname, const bool&, unsigned nbr = 1);
    void SetUniform(const std::string& uname, const int&, unsigned nbr = 1);
    void SetUniform(const std::string& uname, const unsigned&, unsigned nbr = 1);
    void SetUniform(const std::string& uname, const float&, unsigned nbr = 1);
    void SetUniform(const std::string& uname, const glm::vec2&, unsigned nbr = 1);
    void SetUniform(const std::string& uname, const glm::vec3&, unsigned nbr = 1);
    void SetUniform(const std::string& uname, const glm::mat4&, unsigned nbr = 1);
    void use(const bool& use_program = true);
    ShaderVariable* get_uniform(const std::string& name);
    ShaderVariable* get_attribute(const std::string& name);
    bool in_use();
    bool Compiled() const;
    bool NeedsRecompile() const;
    void Recompile();
    void SetDefine(const std::string define, const std::string value = "");
    void RemoveDefine(const std::string define);
    ShaderStage &Stage(GLenum stage);
    void SetStage(const ShaderStage &stage);
    void RemoveStage(GLenum stage);
    ShaderType Type();
    //void SetType(ShaderType);
    void Compile();
    void Link();

protected:

private:
    Shader(const std::string& name);
    static std::vector<std::shared_ptr<Shader>> _shaders;
    GLuint _program { 0 };
    bool _in_use { false };
    bool _compiled { false };
    bool _needsRecompile { false };
    std::unordered_map<GLenum, ShaderStage> _shaderStages;
    std::unordered_map<std::string, ShaderVariable> _get_variables(GLenum type);
    std::unordered_map<std::string, ShaderVariable> _uniforms;
    std::unordered_map<std::string, ShaderVariable> _attributes;
    std::unordered_map<std::string, std::string> _defines;
    ShaderType _type;
};
