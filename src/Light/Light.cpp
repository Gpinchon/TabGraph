/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-16 22:06:28
*/

#include "Light/Light.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Config.hpp" // for Config
#include "Framebuffer.hpp" // for Framebuffer
#include "Scene/Scene.hpp"
#include "Texture/Texture.hpp" // for Texture
#include "Transform.hpp"
#include <GL/glew.h> // for GL_COMPARE_REF_TO_TEXTURE, GL_DEPTH_COMPO...
#include <glm/ext.hpp>

Light::Light(const std::string& name)
    : Node(name)
{
}

std::shared_ptr<Light> Light::Create(const std::string& name, glm::vec3 color, glm::vec3 position, float power)
{
    auto light = std::shared_ptr<Light>(new Light(name));
    light->color() = color;
    light->power() = power;
    light->GetComponent<Transform>()->SetPosition(position);
    return (light);
}

/*TextureArray	*Light::shadow_array()
{
	if (nullptr == _shadow_array)
		_shadow_array = TextureArray::Create("ShadowArray", glm::vec2(Config::ShadowRes(), Config::ShadowRes()), GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT24, 128);
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

glm::mat4 Light::ShadowProjectionMatrix() const
{
    return glm::mat4(1.0);
}

void Light::render_shadow()
{
}

DirectionnalLight::DirectionnalLight(const std::string& name)
    : Light(name)
{
}

std::shared_ptr<DirectionnalLight> DirectionnalLight::Create(const std::string& name, glm::vec3 color, glm::vec3 position, float power, bool cast_shadow)
{
    auto light = std::shared_ptr<DirectionnalLight>(new DirectionnalLight(name));
    light->color() = color;
    light->GetComponent<Transform>()->SetPosition(position);
    light->power() = power;
    light->cast_shadow() = cast_shadow;
    if (cast_shadow) {
        light->_render_buffer = Framebuffer::Create(light->Name() + "_shadowMap", glm::vec2(Config::Get("ShadowRes", 1024)), 0, 0);
        auto renderBuffer = light->_render_buffer.lock();
        renderBuffer->Create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24);
        renderBuffer->depth()->set_parameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        renderBuffer->depth()->set_parameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        //shadow_array()->Add(light->_render_buffer->depth());
        //shadow_array()->load();
    }
    return (light);
}

void DirectionnalLight::render_shadow()
{
    auto camera = Scene::Current()->CurrentCamera();
    static auto tempCamera = Camera::Create("light_camera", 45, Camera::Projection::Ortho);
    GetComponent<Transform>()->LookAt(glm::vec3(0));
    Scene::Current()->SetCurrentCamera(tempCamera);
    render_buffer()->bind();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_DEPTH_BUFFER_BIT);
    tempCamera->SetZnear(0.001f);
    tempCamera->SetZfar(100);
    tempCamera->SetFrustum(Limits());
    tempCamera->AddComponent(GetComponent<Transform>());
    //tempCamera->SetPosition(Position());
    //tempCamera->SetViewMatrix(glm::inverse(glm::lookAt(Position(), glm::vec3(0, 0, 0), Common::Up())));
    Scene::Current()->RenderDepth(RenderMod::RenderOpaque);
    render_buffer()->bind(false);
    Scene::Current()->SetCurrentCamera(camera);
}

//void DirectionnalLight::UpdateTransformMatrix()
//{
//    SetTransformMatrix(glm::lookAt(Position(), glm::vec3(0, 0, 0), Common::Up()));
//}

glm::mat4 DirectionnalLight::ShadowProjectionMatrix() const
{
    return glm::ortho(Limits().x, Limits().y, Limits().z, Limits().w, 0.001f, 100.f) * glm::lookAt(GetComponent<Transform>()->WorldPosition(), glm::vec3(0, 0, 0), Common::Up());
}

LightType DirectionnalLight::type()
{
    return (Directionnal);
}

void DirectionnalLight::SetLimits(glm::vec4 limits)
{
    _limits = limits;
}

glm::vec4 DirectionnalLight::Limits() const
{
    return _limits;
}
