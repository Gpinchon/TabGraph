#include "Material/MaterialExtension.hpp"
#include "Shader/Stage.hpp"

void MaterialExtension::SetCode(const Shader::Stage::Code& code)
{
	_code = code;
}

void MaterialExtension::SetColor(const std::string& name, const glm::vec3& value)
{
	_colors[name] = value;
}

void MaterialExtension::SetValue(const std::string& name, const float value)
{
	_values[name] = value;
}

void MaterialExtension::SetDefine(const std::string& name, const std::string& value)
{
	_defines[name] = value;
}

void MaterialExtension::RemoveDefine(const std::string& name)
{
	_defines.erase(name);
}

void MaterialExtension::SetTexture(const std::string& name, const std::shared_ptr<Texture2D>& value)
{
	_textures[name] = value;
}

const Shader::Stage::Code& MaterialExtension::GetCode() const
{
	return _code;
}

const glm::vec3 MaterialExtension::GetColor(const std::string& name) const
{
	auto value{ _colors.find(name) };
	return value == _colors.end() ? glm::vec3(0) : value->second;
}

const float MaterialExtension::GetValue(const std::string& name) const
{
	auto value{ _values.find(name) };
	return value == _values.end() ? float(0) : value->second;
}

const std::string MaterialExtension::GetDefine(const std::string& name) const
{
	auto value{ _defines.find(name) };
	return value == _defines.end() ? "" : value->second;
}

const std::shared_ptr<Texture2D> MaterialExtension::GetTexture(const std::string& name) const
{
	auto value{ _textures.find(name) };
	return value == _textures.end() ? nullptr : value->second;
}

const MaterialExtension::Colors& MaterialExtension::GetColors() const
{
	return _colors;
}

const  MaterialExtension::Values& MaterialExtension::GetValues() const
{
	return _values;
}

const  MaterialExtension::Defines& MaterialExtension::GetDefines() const
{
	return _defines;
}

const MaterialExtension::Textures& MaterialExtension::GetTextures() const
{
	return _textures;
}

MaterialExtension::MaterialExtension(const std::string& name) : Component(name)
{
}
