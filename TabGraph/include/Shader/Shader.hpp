/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-21 21:49:39
*/

#pragma once

#include "Component.hpp" // for Component
#include "Shader/ShaderStage.hpp"
#include "Tools/Tools.hpp"
#include <GL/glew.h> // for GLuint, GLenum, GLint
#include <cstdlib>
#include <functional>
#include <glm/glm.hpp> // for glm::mat4, glm::vec2, glm::vec3
#include <memory> // for shared_ptr
#include <stddef.h> // for size_t
#include <string> // for string
#include <map> // for unordered_map
#include <variant>
#include <vector> // for vector

class Texture;

struct ShaderVariable {
    std::string name { "" };
    size_t size { 0 };
    GLenum type { 0 };
    GLint loc { -1 };
    std::string typeName{};
    size_t byteSize { 0 };
    std::function<void(const ShaderVariable&)> updateFunction {};
    template <typename T, typename = IsNotSharedPointerOfType<Texture, T>>
    void Set(const T& value, const size_t index = 0);
    //template<typename T, typename std::enable_if<std::is_convertible<T, std::shared_ptr<Texture>>::value> * = nullptr>
    void Set(const std::shared_ptr<Texture> value, const size_t index = 0);
    std::variant<
        //std::shared_ptr<Texture>,
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
        std::vector<glm::dmat4x3>>
        data;
    //std::variant<bool, int, unsigned, float, glm::vec2, glm::vec3, glm::mat4, std::pair<std::shared_ptr<Texture>, GLenum>> value;
};

class ShaderExtension;

class Shader : public Component {
public:
    enum class Type {
        Invalid = -1,
        Other,
        ForwardGeometryShader,
        ForwardMaterialShader,
        LightingShader,
        PostShader,
        ComputeShader
    };
    Shader(const std::string& name, const Shader::Type& type = Shader::Type::Other);
    //static std::shared_ptr<Shader> Get(unsigned index);
    //static std::shared_ptr<Shader> GetByName(const std::string&);
    void AddExtension(const std::shared_ptr<ShaderExtension>& extension);
    void RemoveExtension(const std::shared_ptr<ShaderExtension>& extension);
    static bool check_shader(const GLuint id);
    static bool check_program(const GLuint id);
    void bind_image(const std::string& uname, std::shared_ptr<Texture> texture, const GLint level, const bool layered, const GLint layer, const GLenum access, const GLenum texture_unit);
    void unbind_texture(GLenum texture_unit);
    void SetAttribute(const ShaderVariable& attribute);
    void SetUniform(const ShaderVariable& uniform);
    void SetTexture(const ShaderVariable& texture);
    template <typename T, typename = IsNotSharedPointerOfType<Texture, T>>
    void SetUniform(const std::string& uname, const T& value, const size_t index = 0);
    void SetTexture(const std::string& uname, const std::shared_ptr<Texture>& value);

    template <typename T, typename = IsNotSharedPointerOfType<Texture, T>>
    static void SetGlobalUniform(const std::string& uname, const T& value, const size_t index = 0);
    static void SetGlobalTexture(const std::string& uname, const std::shared_ptr<Texture>& value);
    static void SetGlobalDefine(const std::string define, const std::string value = "");
    static void RemoveGlobalDefine(const std::string define);

    void use(const bool& use_program = true);
    std::unordered_map<std::string, ShaderVariable> Textures() const;
    std::unordered_map<std::string, ShaderVariable> Uniforms() const;
    std::unordered_map<std::string, ShaderVariable> Attributes() const;
    ShaderVariable& get_uniform(const std::string& name);
    ShaderVariable& get_attribute(const std::string& name);
    bool in_use();
    bool Compiled() const;
    bool NeedsRecompile() const;
    void Recompile();
    std::unordered_map<std::string, std::string> Defines() const;
    void SetDefine(const std::string define, const std::string value = "");
    void RemoveDefine(const std::string define);
    std::unordered_map<GLenum, std::shared_ptr<ShaderStage>> Stages() const;
    std::shared_ptr<ShaderStage> Stage(GLenum stage) const;
    void SetStage(const std::shared_ptr<ShaderStage>& stage);
    void RemoveStage(GLenum stage);
    Shader::Type GetType();
    void Compile();
    void Link();

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<Shader>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    //static std::vector<std::shared_ptr<Shader>> _shaders;
    GLuint _program { 0 };
    bool _texturesChanged{ false };
    bool _uniformsChanged { false };
    bool _attributesChanged { false };
    bool _compiled { false };
    bool _needsRecompile { false };
    void _UpdateVariables();
    void _UpdateVariable(const ShaderVariable& variable);
    std::unordered_map<GLenum, std::shared_ptr<ShaderStage>> _shaderStages;
    void _get_variables(GLenum type);
    std::unordered_map<std::string, ShaderVariable> _textures;
    std::unordered_map<std::string, ShaderVariable> _uniforms;
    std::unordered_map<std::string, ShaderVariable> _attributes;
    std::unordered_map<std::string, std::string> _defines;
    static std::unordered_map<std::string, ShaderVariable> _globalTextures;
    static std::unordered_map<std::string, ShaderVariable> _globalUniforms;
    static std::unordered_map<std::string, std::string> _globalDefines;
    Shader::Type _type{ Shader::Type::Invalid };
};

#include "Debug.hpp"
#include <algorithm>
#include <cstring>

template <typename T, typename>
inline void ShaderVariable::Set(const T& value, const size_t index)
{
    byteSize = byteSize == 0 ? sizeof(T) : byteSize;
    if (sizeof(T) != byteSize)
        throw std::runtime_error(std::string(__FUNCTION__) + " variable byte size(" + std::to_string(byteSize) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
    auto dataPtr(std::get_if<std::vector<T>>(&data));
    if (dataPtr != nullptr) {
        dataPtr->resize(glm::max(index + 1, dataPtr->size()));
        dataPtr->at(index) = value;
    } else {
        data = std::vector<T>(index + 1);
        std::get<std::vector<T>>(data).at(index) = value;
    }
}

inline void ShaderVariable::Set(const std::shared_ptr<Texture> texture, const size_t index)
{
    auto value(std::pair(texture, index));
    byteSize = sizeof(value);
    data = value;
}

template <typename T, typename>
inline void Shader::SetUniform(const std::string& uname, const T& value, const size_t index)
{
    if (in_use()) {
        debugLog(" Warning setting uniform " + uname + " Shader " + Name() + " in use");
    }
    _uniforms[uname].Set(value, index);
    _uniforms[uname].name = uname;
    _uniformsChanged = true;
}

template<typename T, typename>
inline void Shader::SetGlobalUniform(const std::string& uname, const T& value, const size_t index)
{
    _globalUniforms[uname].Set(value, index);
    _globalUniforms[uname].name = uname;
}
