/*
* @Author: gpinchon
* @Date:   2021-03-14 23:55:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 18:19:52
*/

#include <Assets/Asset.hpp>
#include <Light/HDRLight.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Texture/Sampler.hpp>
#include <Tools/Tools.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Light/HDRLightRenderer.hpp>
#endif

#include <glm/common.hpp>

namespace TabGraph::Lights {
HDRLight::HDRLight(const std::string& name, std::shared_ptr<Assets::Asset> image)
    : Inherit(name)
{
    _renderer.reset(new Renderer::HDRLightRenderer(*this));
    SetHDRImage(image);
}

void HDRLight::SetHDRImage(std::shared_ptr<Assets::Asset> image)
{
    if (image != GetHDRTexture())
        GetRenderer().FlagDirty();
    _SetHDRImage(image);
    if (GetHDRTexture() == nullptr) return;
    _SetHDRTexture(std::make_shared<Textures::TextureCubemap>(image));
    GetHDRTexture()->SetAutoMipMap(true);
    GetHDRTexture()->GetTextureSampler()->SetMinFilter(Textures::Sampler::Filter::LinearMipmapLinear);
}

glm::vec2 ToImageCoords(double phi, double theta, int width, int height)
{
    // Allow theta to repeat and map to 0 to pi. However, to account for cases
    // where y goes beyond the normal 0 to pi range, phi may need to be adjusted.
    theta = glm::clamp(glm::mod(theta, 2.0 * M_PI), 0.0, 2.0 * M_PI);
    if (theta > M_PI) {
        // theta is out of bounds. Effectively, theta has rotated past the pole
        // so after adjusting theta to be in range, rotating phi by pi forms an
        // equivalent direction.
        theta = 2.0 * M_PI - theta; // now theta is between 0 and pi
        phi += M_PI;
    }
    // Allow phi to repeat and map to the normal 0 to 2pi range.
    // Clamp and map after adjusting theta in case theta was forced to update phi.
    phi = glm::clamp(glm::mod(phi, 2.0 * M_PI), 0.0, 2.0 * M_PI);

    // Now phi is in [0, 2pi] and theta is in [0, pi] so it's simple to inverse
    // the linear equations in ImageCoordsToSphericalCoords, although there's no
    // -0.5 because we're returning floating point coordinates and so don't need
    // to center the pixel.
    return glm::vec2(width * phi / (2.0 * M_PI), height * theta / M_PI);
}
}