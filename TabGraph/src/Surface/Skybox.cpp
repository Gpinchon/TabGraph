/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

#include "Surface/Skybox.hpp"
#include "Texture/TextureCubemap.hpp"

//#if RENDERINGAPI == OpenGL
#include "Driver/OpenGL/Renderer/Surface/SkyboxRenderer.hpp"
//#endif

Skybox::Skybox(const std::string& name)
    : Surface(name)
{
    _renderer.reset(new Renderer::SkyboxRenderer(*this));
}

Skybox::Skybox(const std::string& name, std::shared_ptr<TextureCubemap> color)
    : Skybox(name)
{
    SetTexture(color);
}

Skybox::Skybox(const Skybox& other) : Surface(other)
{
    _renderer.reset(new Renderer::SkyboxRenderer(*this));
    SetTexture(other.GetTexture());
}

void Skybox::Load()
{
    GetTexture()->Load();
}

void Skybox::Unload()
{
    GetTexture()->Unload();
}
