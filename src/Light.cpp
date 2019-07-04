/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-28 08:07:29
*/

#include "Light.hpp"
#include "Config.hpp" // for Config
#include "Engine.hpp" // for up()
#include "Framebuffer.hpp" // for Framebuffer
#include "Texture.hpp" // for Texture
#include <GL/glew.h> // for GL_COMPARE_REF_TO_TEXTURE, GL_DEPTH_COMPO...
#include <glm/ext.hpp>

//TextureArray	*Light::_shadow_array = nullptr;
std::vector<std::shared_ptr<Light>> Light::_lights;

Light::Light(const std::string& name)
    : Node(name)
{
}

std::shared_ptr<Light> Light::create(const std::string& name, glm::vec3 color, glm::vec3 position, float power)
{
    auto light = std::shared_ptr<Light>(new Light(name));
    light->color() = color;
    light->position() = position;
    light->power() = power;
    Light::add(light);
    Node::add(light);
    return (light);
}

std::shared_ptr<Light> Light::get_by_name(const std::string& name)
{
    for (auto n : _lights) {
        if (name == n->name())
            return (n);
    }
    return (nullptr);
}

std::shared_ptr<Light> Light::Get(unsigned index)
{
    if (index >= _lights.size())
        return (nullptr);
    return (_lights.at(index));
}

void Light::add(std::shared_ptr<Light> light)
{
    _lights.push_back(light);
}

/*TextureArray	*Light::shadow_array()
{
	if (nullptr == _shadow_array)
		_shadow_array = TextureArray::create("ShadowArray", glm::vec2(Config::ShadowRes(), Config::ShadowRes()), GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT24, 128);
	return (_shadow_array);
}*/

std::shared_ptr<Framebuffer> Light::render_buffer()
{
    return (_render_buffer.lock());
}

glm::vec3& Light::color()
{
    return (_color);
}

float& Light::power()
{
    return (_power);
}

bool& Light::cast_shadow()
{
    return (_cast_shadow);
}

LightType Light::type()
{
    return (Point);
}

void Light::render_shadow()
{
}

DirectionnalLight::DirectionnalLight(const std::string& name)
    : Light(name)
{
}

std::shared_ptr<DirectionnalLight> DirectionnalLight::create(const std::string& name, glm::vec3 color, glm::vec3 position, float power, bool cast_shadow)
{
    auto light = std::shared_ptr<DirectionnalLight>(new DirectionnalLight(name));
    light->color() = color;
    light->position() = position;
    light->power() = power;
    light->cast_shadow() = cast_shadow;
    if (cast_shadow) {
        light->_render_buffer = Framebuffer::create(light->name() + "_shadowMap", glm::vec2(Config::ShadowRes(), Config::ShadowRes()), 0, 0);
        auto renderBuffer = light->_render_buffer.lock();
        renderBuffer->create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24);
        renderBuffer->depth()->set_parameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        renderBuffer->depth()->set_parameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        //shadow_array()->add(light->_render_buffer->depth());
        //shadow_array()->load();
    }
    Light::add(light);
    Node::add(light);
    return (light);
}

void DirectionnalLight::render_shadow()
{
}

void DirectionnalLight::transform_update()
{
    auto proj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
    transform() = proj * glm::lookAt(position(), glm::vec3(0, 0, 0), up());
}

LightType DirectionnalLight::type()
{
    return (Directionnal);
}
