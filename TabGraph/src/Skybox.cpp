/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 18:53:46
*/

#include "Skybox.hpp"
#include "Renderer/SkyboxRenderer.hpp"
#include "Texture/Cubemap.hpp"

Skybox::Skybox(const std::string& name)
    : Component(name)
    , _renderer(new Renderer::SkyboxRenderer(*this))
{
}

Skybox::Skybox(const std::string& name, std::shared_ptr<Cubemap> color)
    : Skybox(name)
{
    SetTexture(color);
}

Skybox::Skybox(const Skybox& other)
    : _renderer(new Renderer::SkyboxRenderer(*this))
{
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

Renderer::SkyboxRenderer& Skybox::GetRenderer()
{
    return *_renderer;
}
