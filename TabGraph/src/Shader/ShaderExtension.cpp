#include "Shader\ShaderExtension.hpp"

ShaderExtension::ShaderExtension(const std::string& name) : Shader(name)
{
}

std::shared_ptr<ShaderExtension> ShaderExtension::Create(const std::string& name)
{
	return std::shared_ptr<ShaderExtension>(new ShaderExtension(name));
}
