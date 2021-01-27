#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include <glm/gtc/type_ptr.hpp>

void SetUniform1fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<GLfloat>>(&variable.data));
    if (value != nullptr)
        glUniform1fv(variable.loc, glm::min(value->size(), variable.size), value->data());
}

void SetUniform2fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::vec2>>(&variable.data));
    if (value != nullptr)
        glUniform2fv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform3fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::vec3>>(&variable.data));
    if (value != nullptr)
        glUniform3fv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform4fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::vec4>>(&variable.data));
    if (value != nullptr)
        glUniform4fv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform1dv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<double>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<float> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    glUniform1fv(variable.loc, v.size(), v.data());
}

void SetUniform2dv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::dvec2>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<glm::vec2> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    glUniform2fv(variable.loc, v.size(), glm::value_ptr(v.at(0)));
}

void SetUniform3dv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::dvec3>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<glm::vec3> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    glUniform3fv(variable.loc, v.size(), glm::value_ptr(v.at(0)));
}

void SetUniform4dv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::dvec4>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<glm::vec4> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    glUniform4fv(variable.loc, v.size(), glm::value_ptr(v.at(0)));
}

void SetUniform1bv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<bool>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<GLint> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    if (value != nullptr)
        glUniform1iv(variable.loc, glm::min(value->size(), variable.size), v.data());
}

void SetUniform2bv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::bvec2>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<glm::ivec2> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    if (value != nullptr)
        glUniform2iv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr(v.at(0)));
}

void SetUniform3bv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::bvec3>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<glm::ivec3> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    if (value != nullptr)
        glUniform3iv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr(v.at(0)));
}

void SetUniform4bv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::bvec4>>(&variable.data));
    if (value == nullptr)
        return;
    std::vector<glm::ivec4> v(glm::min(value->size(), variable.size));
    for (auto i = 0u; i < v.size(); ++i)
        v[i] = value->at(i);
    if (value != nullptr)
        glUniform4iv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr(v.at(0)));
}

void SetUniform1iv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<GLint>>(&variable.data));
    if (value != nullptr)
        glUniform1iv(variable.loc, glm::min(value->size(), variable.size), value->data());
}

void SetUniform2iv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::ivec2>>(&variable.data));
    if (value != nullptr)
        glUniform2iv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform3iv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::ivec3>>(&variable.data));
    if (value != nullptr)
        glUniform3iv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform4iv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::ivec4>>(&variable.data));
    if (value != nullptr)
        glUniform4iv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform1uiv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<GLuint>>(&variable.data));
    if (value != nullptr)
        glUniform1uiv(variable.loc, glm::min(value->size(), variable.size), value->data());
}

void SetUniform2uiv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::uvec2>>(&variable.data));
    if (value != nullptr)
        glUniform2uiv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform3uiv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::uvec3>>(&variable.data));
    if (value != nullptr)
        glUniform3uiv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniform4uiv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::uvec4>>(&variable.data));
    if (value != nullptr)
        glUniform4uiv(variable.loc, glm::min(value->size(), variable.size), glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix2fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat2>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix2fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix3fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat3>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix3fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix4fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat4>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix4fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix2x3fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<GLfloat>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix2x3fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, value->data());
}

void SetUniformMatrix3x2fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat3x2>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix3x2fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix2x4fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat2x4>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix2x4fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix4x2fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat4x2>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix4x2fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix3x4fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat3x4>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix3x4fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix4x3fv (const ShaderVariable &variable) {
    auto value(std::get_if<std::vector<glm::mat4x3>>(&variable.data));
    if (value != nullptr)
        glUniformMatrix4x3fv(variable.loc, glm::min(value->size(), variable.size), GL_FALSE, glm::value_ptr((*value).at(0)));
}

void SetUniformMatrix2dv(const ShaderVariable &)
{

}

void SetUniformMatrix3dv(const ShaderVariable &)
{

}

void SetUniformMatrix4dv(const ShaderVariable &)
{

}

void SetUniformMatrix2x3dv(const ShaderVariable &)
{

}

void SetUniformMatrix2x4dv(const ShaderVariable &)
{

}

void SetUniformMatrix3x2dv(const ShaderVariable &)
{

}

void SetUniformMatrix3x4dv(const ShaderVariable &)
{

}

void SetUniformMatrix4x2dv(const ShaderVariable &)
{

}

void SetUniformMatrix4x3dv(const ShaderVariable &)
{

}


void SetUniformSampler(const ShaderVariable &variable)
{
    auto value(std::get_if<std::pair<std::shared_ptr<Texture>, GLenum>>(&variable.data));
    if (value == nullptr || value->second == 0)
        return;
    glActiveTexture(value->second);
    if (value->first != nullptr) {
        value->first->Load();
        glBindTexture((GLenum)value->first->GetType(), value->first->GetHandle());
    }
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(variable.loc, value->second - GL_TEXTURE0);
}

bool IsTextureType(GLenum type)
{
    switch (type) {
        case (GL_SAMPLER_1D):
        case (GL_SAMPLER_2D):
        case (GL_SAMPLER_3D):
        case (GL_SAMPLER_CUBE):
        case (GL_SAMPLER_1D_SHADOW):
        case (GL_SAMPLER_2D_SHADOW):
        case (GL_SAMPLER_1D_ARRAY):
        case (GL_SAMPLER_2D_ARRAY):
        case (GL_SAMPLER_1D_ARRAY_SHADOW):
        case (GL_SAMPLER_2D_ARRAY_SHADOW):
        case (GL_SAMPLER_2D_MULTISAMPLE):
        case (GL_SAMPLER_2D_MULTISAMPLE_ARRAY):
        case (GL_SAMPLER_CUBE_SHADOW):
        case (GL_SAMPLER_BUFFER):
        case (GL_SAMPLER_2D_RECT):
        case (GL_SAMPLER_2D_RECT_SHADOW):
        case (GL_INT_SAMPLER_1D):
        case (GL_INT_SAMPLER_2D):
        case (GL_INT_SAMPLER_3D):
        case (GL_INT_SAMPLER_CUBE):
        case (GL_INT_SAMPLER_1D_ARRAY):
        case (GL_INT_SAMPLER_2D_ARRAY):
        case (GL_INT_SAMPLER_2D_MULTISAMPLE):
        case (GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):
        case (GL_INT_SAMPLER_BUFFER):
        case (GL_INT_SAMPLER_2D_RECT):
        case (GL_UNSIGNED_INT_SAMPLER_1D):
        case (GL_UNSIGNED_INT_SAMPLER_2D):
        case (GL_UNSIGNED_INT_SAMPLER_3D):
        case (GL_UNSIGNED_INT_SAMPLER_CUBE):
        case (GL_UNSIGNED_INT_SAMPLER_1D_ARRAY):
        case (GL_UNSIGNED_INT_SAMPLER_2D_ARRAY):
        case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE):
        case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):
        case (GL_UNSIGNED_INT_SAMPLER_BUFFER):
        case (GL_UNSIGNED_INT_SAMPLER_2D_RECT):
        case (GL_IMAGE_1D):
        case (GL_IMAGE_2D):
        case (GL_IMAGE_3D):
        case (GL_IMAGE_2D_RECT):
        case (GL_IMAGE_CUBE):
        case (GL_IMAGE_BUFFER):
        case (GL_IMAGE_1D_ARRAY):
        case (GL_IMAGE_2D_ARRAY):
        case (GL_IMAGE_2D_MULTISAMPLE):
        case (GL_IMAGE_2D_MULTISAMPLE_ARRAY):
        case (GL_INT_IMAGE_1D):
        case (GL_INT_IMAGE_2D):
        case (GL_INT_IMAGE_3D):
        case (GL_INT_IMAGE_2D_RECT):
        case (GL_INT_IMAGE_CUBE):
        case (GL_INT_IMAGE_BUFFER):
        case (GL_INT_IMAGE_1D_ARRAY):
        case (GL_INT_IMAGE_2D_ARRAY):
        case (GL_INT_IMAGE_2D_MULTISAMPLE):
        case (GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY):
        case (GL_UNSIGNED_INT_IMAGE_1D):
        case (GL_UNSIGNED_INT_IMAGE_2D):
        case (GL_UNSIGNED_INT_IMAGE_3D):
        case (GL_UNSIGNED_INT_IMAGE_2D_RECT):
        case (GL_UNSIGNED_INT_IMAGE_CUBE):
        case (GL_UNSIGNED_INT_IMAGE_BUFFER):
        case (GL_UNSIGNED_INT_IMAGE_1D_ARRAY):
        case (GL_UNSIGNED_INT_IMAGE_2D_ARRAY):
        case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE):
        case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY):
            return true;
        default:
            return false;
    }
}

typedef void (*SetUniformFunction)(const ShaderVariable&);

SetUniformFunction GetSetUniformCallback(GLenum type)
{
    switch (type) {
        case(GL_FLOAT) :
            return SetUniform1fv;
        case(GL_FLOAT_VEC2) :
            return SetUniform2fv;
        case(GL_FLOAT_VEC3) :
            return SetUniform3fv;
        case(GL_FLOAT_VEC4) :
            return SetUniform4fv;
        case(GL_DOUBLE) :
            return SetUniform1dv;
        case(GL_DOUBLE_VEC2) :
            return SetUniform2dv;
        case(GL_DOUBLE_VEC3) :
            return SetUniform3dv;
        case(GL_DOUBLE_VEC4) :
            return SetUniform4dv;
        case(GL_INT) :
            return SetUniform1iv;
        case(GL_INT_VEC2) :
            return SetUniform2iv;
        case(GL_INT_VEC3) :
            return SetUniform3iv;
        case(GL_INT_VEC4) :
            return SetUniform4iv;
        case(GL_UNSIGNED_INT) :
            return SetUniform1uiv;
        case(GL_UNSIGNED_INT_VEC2) :
            return SetUniform2uiv;
        case(GL_UNSIGNED_INT_VEC3) :
            return SetUniform3uiv;
        case(GL_UNSIGNED_INT_VEC4) :
            return SetUniform4uiv;
        case(GL_BOOL) :
            return SetUniform1bv;
        case(GL_BOOL_VEC2) :
            return SetUniform2bv;
        case(GL_BOOL_VEC3) :
            return SetUniform3bv;
        case(GL_BOOL_VEC4) :
            return SetUniform4bv;
        case(GL_FLOAT_MAT2) :
            return SetUniformMatrix2fv;
        case(GL_FLOAT_MAT3) :
            return SetUniformMatrix3fv;
        case(GL_FLOAT_MAT4) :
            return SetUniformMatrix4fv;
        case(GL_FLOAT_MAT2x3) :
            return SetUniformMatrix2x3fv;
        case(GL_FLOAT_MAT2x4) :
            return SetUniformMatrix2x4fv;
        case(GL_FLOAT_MAT3x2) :
            return SetUniformMatrix3x2fv;
        case(GL_FLOAT_MAT3x4) :
            return SetUniformMatrix3x4fv;
        case(GL_FLOAT_MAT4x2) :
            return SetUniformMatrix4x2fv;
        case(GL_FLOAT_MAT4x3) :
            return SetUniformMatrix4x3fv;
        case(GL_DOUBLE_MAT2) :
            return SetUniformMatrix2dv;
        case(GL_DOUBLE_MAT3) :
            return SetUniformMatrix3dv;
        case(GL_DOUBLE_MAT4) :
            return SetUniformMatrix4dv;
        case(GL_DOUBLE_MAT2x3) :
            return SetUniformMatrix2x3dv;
        case(GL_DOUBLE_MAT2x4) :
            return SetUniformMatrix2x4dv;
        case(GL_DOUBLE_MAT3x2) :
            return SetUniformMatrix3x2dv;
        case(GL_DOUBLE_MAT3x4) :
            return SetUniformMatrix3x4dv;
        case(GL_DOUBLE_MAT4x2) :
            return SetUniformMatrix4x2dv;
        case(GL_DOUBLE_MAT4x3) :
            return SetUniformMatrix4x3dv;
        case(GL_UNSIGNED_INT_ATOMIC_COUNTER) :
            return SetUniform1uiv;
        case (GL_SAMPLER_1D) :
        case (GL_SAMPLER_2D) :
        case (GL_SAMPLER_3D) :
        case (GL_SAMPLER_CUBE) :
        case (GL_SAMPLER_1D_SHADOW) :
        case (GL_SAMPLER_2D_SHADOW) :
        case (GL_SAMPLER_1D_ARRAY) :
        case (GL_SAMPLER_2D_ARRAY) :
        case (GL_SAMPLER_1D_ARRAY_SHADOW) :
        case (GL_SAMPLER_2D_ARRAY_SHADOW) :
        case (GL_SAMPLER_2D_MULTISAMPLE) :
        case (GL_SAMPLER_2D_MULTISAMPLE_ARRAY) :
        case (GL_SAMPLER_CUBE_SHADOW) :
        case (GL_SAMPLER_BUFFER) :
        case (GL_SAMPLER_2D_RECT) :
        case (GL_SAMPLER_2D_RECT_SHADOW) :
        case (GL_INT_SAMPLER_1D) :
        case (GL_INT_SAMPLER_2D) :
        case (GL_INT_SAMPLER_3D) :
        case (GL_INT_SAMPLER_CUBE) :
        case (GL_INT_SAMPLER_1D_ARRAY) :
        case (GL_INT_SAMPLER_2D_ARRAY) :
        case (GL_INT_SAMPLER_2D_MULTISAMPLE) :
        case (GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) :
        case (GL_INT_SAMPLER_BUFFER) :
        case (GL_INT_SAMPLER_2D_RECT) :
        case (GL_UNSIGNED_INT_SAMPLER_1D) :
        case (GL_UNSIGNED_INT_SAMPLER_2D) :
        case (GL_UNSIGNED_INT_SAMPLER_3D) :
        case (GL_UNSIGNED_INT_SAMPLER_CUBE) :
        case (GL_UNSIGNED_INT_SAMPLER_1D_ARRAY) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) :
        case (GL_UNSIGNED_INT_SAMPLER_BUFFER) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_RECT) :
        case (GL_IMAGE_1D) :
        case (GL_IMAGE_2D) :
        case (GL_IMAGE_3D) :
        case (GL_IMAGE_2D_RECT) :
        case (GL_IMAGE_CUBE) :
        case (GL_IMAGE_BUFFER) :
        case (GL_IMAGE_1D_ARRAY) :
        case (GL_IMAGE_2D_ARRAY) :
        case (GL_IMAGE_2D_MULTISAMPLE) :
        case (GL_IMAGE_2D_MULTISAMPLE_ARRAY) :
        case (GL_INT_IMAGE_1D) :
        case (GL_INT_IMAGE_2D) :
        case (GL_INT_IMAGE_3D) :
        case (GL_INT_IMAGE_2D_RECT) :
        case (GL_INT_IMAGE_CUBE) :
        case (GL_INT_IMAGE_BUFFER) :
        case (GL_INT_IMAGE_1D_ARRAY) :
        case (GL_INT_IMAGE_2D_ARRAY) :
        case (GL_INT_IMAGE_2D_MULTISAMPLE) :
        case (GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY) :
        case (GL_UNSIGNED_INT_IMAGE_1D) :
        case (GL_UNSIGNED_INT_IMAGE_2D) :
        case (GL_UNSIGNED_INT_IMAGE_3D) :
        case (GL_UNSIGNED_INT_IMAGE_2D_RECT) :
        case (GL_UNSIGNED_INT_IMAGE_CUBE) :
        case (GL_UNSIGNED_INT_IMAGE_BUFFER) :
        case (GL_UNSIGNED_INT_IMAGE_1D_ARRAY) :
        case (GL_UNSIGNED_INT_IMAGE_2D_ARRAY) :
        case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE) :
        case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) :
            return SetUniformSampler;
    }
    return (0);
}