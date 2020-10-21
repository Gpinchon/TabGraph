#pragma once
#include <GL/glew.h>
#include <functional>

class ShaderVariable;

typedef void (*SetUniformFunction)(const ShaderVariable&);

bool IsTextureType(GLenum type);

//std::function<void(const ShaderVariable&)> GetSetUniformCallback(GLenum type);
SetUniformFunction GetSetUniformCallback(GLenum type);
void SetUniform1fv (const ShaderVariable &variable);
void SetUniform2fv (const ShaderVariable &variable);
void SetUniform3fv (const ShaderVariable &variable);
void SetUniform4fv (const ShaderVariable &variable);
void SetUniform1dv (const ShaderVariable &variable);
void SetUniform2dv (const ShaderVariable &variable);
void SetUniform3dv (const ShaderVariable &variable);
void SetUniform4dv (const ShaderVariable &variable);
void SetUniform1iv (const ShaderVariable &variable);
void SetUniform2iv (const ShaderVariable &variable);
void SetUniform3iv (const ShaderVariable &variable);
void SetUniform4iv (const ShaderVariable &variable);
void SetUniform1uiv (const ShaderVariable &variable);
void SetUniform2uiv (const ShaderVariable &variable);
void SetUniform3uiv (const ShaderVariable &variable);
void SetUniform4uiv (const ShaderVariable &variable);
void SetUniformMatrix2fv (const ShaderVariable &variable);
void SetUniformMatrix3fv (const ShaderVariable &variable);
void SetUniformMatrix4fv (const ShaderVariable &variable);
void SetUniformMatrix2x3fv (const ShaderVariable &variable);
void SetUniformMatrix3x2fv (const ShaderVariable &variable);
void SetUniformMatrix2x4fv (const ShaderVariable &variable);
void SetUniformMatrix4x2fv (const ShaderVariable &variable);
void SetUniformMatrix3x4fv (const ShaderVariable &variable);
void SetUniformMatrix4x3fv (const ShaderVariable &variable);
void SetUniformMatrix2dv (const ShaderVariable &variable);
void SetUniformMatrix3dv (const ShaderVariable &variable);
void SetUniformMatrix4dv (const ShaderVariable &variable);
void SetUniformMatrix2x3dv (const ShaderVariable &variable);
void SetUniformMatrix3x2dv (const ShaderVariable &variable);
void SetUniformMatrix2x4dv (const ShaderVariable &variable);
void SetUniformMatrix4x2dv (const ShaderVariable &variable);
void SetUniformMatrix3x4dv (const ShaderVariable &variable);
void SetUniformMatrix4x3dv (const ShaderVariable &variable);
void SetUniformSampler (const ShaderVariable &variable);