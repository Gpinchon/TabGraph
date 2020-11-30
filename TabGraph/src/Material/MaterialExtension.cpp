#include "Material/MaterialExtension.hpp"
//#include "Shader/Shader.hpp"
#include "Shader/ShaderExtension.hpp"

MaterialExtension::MaterialExtension(const std::string& name) : Component(name)
{
	SetComponent(Component::Create<ShaderExtension>("ShaderExtension"));
}

std::shared_ptr<ShaderExtension> MaterialExtension::GetShaderExtension() const
{
	return GetComponent<ShaderExtension>();
}
