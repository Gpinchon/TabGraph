/*
* @Author: gpinchon
* @Date:   2021-03-24 16:07:09
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 16:07:09
*/

#include "Renderer/SkyboxRenderer.hpp"

#include "Skybox.hpp"

namespace Renderer {
void Render(std::shared_ptr<Skybox> skybox, const Renderer::Options& options)
{
    skybox->GetRenderer().Render(options);
}
};