#include "Shader\ShaderExtension.hpp"

ShaderExtension::ShaderExtension(const std::string& name) : Shader(name)
{
}

std::shared_ptr<ShaderExtension> ShaderExtension::Create(const std::string& name)
{
	return tools::make_shared<ShaderExtension>(name);
}
