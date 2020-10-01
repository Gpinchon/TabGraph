#include "Component.hpp"

class Texture2D;
class Shader;

class MaterialExtension : public Component
{
public:
	virtual std::shared_ptr<Shader> ShaderExtension() = 0;

private:
	
};