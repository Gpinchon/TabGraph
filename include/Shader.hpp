/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:09
*/

#pragma once

#include "Object.hpp" // for Object
#include "ShaderStage.hpp"
#include "glm/glm.hpp" // for glm::mat4, glm::vec2, glm::vec3
#include <GL/glew.h> // for GLuint, GLenum, GLint
#include <memory> // for shared_ptr
#include <stddef.h> // for size_t
#include <string> // for string
#include <unordered_map> // for unordered_map
#include <vector> // for vector
#include <variant>
#include <functional>
#include <cstdlib>

class Texture; // lines 16-16

struct ShaderVariable {
    std::string name { "" };
    size_t size{ 0 };
    GLenum type{ 0 };
    GLint loc { -1 };
    size_t byteSize{ 0 };
    //std::byte *data{ nullptr };
    std::function<void(const ShaderVariable&)> updateFunction {};
    template<typename T>
    typename std::enable_if<!std::is_constructible<std::shared_ptr<Texture>, T>::value>::type Set(const T &value, const size_t index = 0);
    //template<typename T, typename std::enable_if<std::is_convertible<T, std::shared_ptr<Texture>>::value> * = nullptr>
    void Set(const std::shared_ptr<Texture> value, const size_t index = 0);
    std::variant<
        std::pair<std::shared_ptr<Texture>, GLenum>,
        std::vector<float>,
        std::vector<glm::vec2>,
        std::vector<glm::vec3>,
        std::vector<glm::vec4>,
        std::vector<double>,
        std::vector<glm::dvec2>,
        std::vector<glm::dvec3>,
        std::vector<glm::dvec4>,
        std::vector<int>,
        std::vector<glm::ivec2>,
        std::vector<glm::ivec3>,
        std::vector<glm::ivec4>,
        std::vector<unsigned>,
        std::vector<glm::uvec2>,
        std::vector<glm::uvec3>,
        std::vector<glm::uvec4>,
        std::vector<bool>,
        std::vector<glm::bvec2>,
        std::vector<glm::bvec3>,
        std::vector<glm::bvec4>,
        std::vector<glm::mat2>,
        std::vector<glm::mat3>,
        std::vector<glm::mat4>,
        std::vector<glm::mat2x3>,
        std::vector<glm::mat2x4>,
        std::vector<glm::mat3x2>,
        std::vector<glm::mat3x4>,
        std::vector<glm::mat4x2>,
        std::vector<glm::mat4x3>,
        std::vector<glm::dmat2>,
        std::vector<glm::dmat3>,
        std::vector<glm::dmat4>,
        std::vector<glm::dmat2x3>,
        std::vector<glm::dmat2x4>,
        std::vector<glm::dmat3x2>,
        std::vector<glm::dmat3x4>,
        std::vector<glm::dmat4x2>,
        std::vector<glm::dmat4x3>> data;
    //std::variant<bool, int, unsigned, float, glm::vec2, glm::vec3, glm::mat4, std::pair<std::shared_ptr<Texture>, GLenum>> value;
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
    //void SetUniform(const std::string& uname, std::shared_ptr<Texture>, const GLenum texture_unit);
    void bind_image(const std::string& uname, std::shared_ptr<Texture> texture, const GLint level, const bool layered, const GLint layer, const GLenum access, const GLenum texture_unit);
    void unbind_texture(GLenum texture_unit);
    template <typename T>
    void SetUniform(const std::string& uname, const T&value, const size_t index = 0);
    void use(const bool& use_program = true);
    ShaderVariable &get_uniform(const std::string& name);
    ShaderVariable &get_attribute(const std::string& name);
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
    void Compile();
    void Link();

protected:

private:
    Shader(const std::string& name);
    static std::vector<std::shared_ptr<Shader>> _shaders;
    GLuint _program { 0 };
    bool _uniformsChanged { false };
    bool _attributesChanged { false };
    bool _compiled { false };
    bool _needsRecompile { false };
    void _UpdateVariables();
    void _UpdateVariable(const ShaderVariable &variable);
    std::unordered_map<GLenum, ShaderStage> _shaderStages;
    void _get_variables(GLenum type);
    std::unordered_map<std::string, ShaderVariable> _uniforms;
    std::unordered_map<std::string, ShaderVariable> _attributes;
    std::unordered_map<std::string, std::string> _defines;
    ShaderType _type;
};

#include "Debug.hpp"
#include <cstring>
#include <algorithm>

template<typename T>
inline typename std::enable_if<!std::is_constructible<std::shared_ptr<Texture>, T>::value>::type ShaderVariable::Set(const T &value, const size_t index)
{
    byteSize = byteSize == 0 ? sizeof(T) : byteSize;
    if (sizeof(T) != byteSize)
        throw std::runtime_error(std::string(__FUNCTION__) + " variable byte size(" + std::to_string(byteSize) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
    auto dataPtr(std::get_if<std::vector<T>>(&data));
    if (dataPtr != nullptr) {
        dataPtr->resize(glm::max(index + 1, dataPtr->size()));
        dataPtr->at(index) = value;
    }
    else {
        data = std::vector<T>(index + 1);
        std::get<std::vector<T>>(data).at(index) = value;
    }
    /*auto dataIndex(index * sizeof(T));
    if (dataIndex >= byteSize * localSize)
        throw std::runtime_error(std::string(__FUNCTION__) + " data index(" + std::to_string(dataIndex) + ") out of bound(" + std::to_string(byteSize * localSize) + ")");
    std::memcpy(std::get_if<T*>(&data)[dataIndex], &value, sizeof(T));*/
}

//template<typename T, typename std::enable_if<std::is_convertible<T, std::shared_ptr<Texture>>::value> *>
inline void ShaderVariable::Set(const std::shared_ptr<Texture> texture, const size_t index)
{
    auto value(std::pair(texture, index));
    byteSize = sizeof(value);
    data = value;
}

template <typename T>
inline void Shader::SetUniform(const std::string& uname, const T&value, const size_t index)
{
    if (in_use()) {
        debugLog(" Warning setting uniform " + uname + " Shader " + Name() + " in use");
    }
    _uniforms[uname].Set(value, index);
    _uniformsChanged = true;
}
