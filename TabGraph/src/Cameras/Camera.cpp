/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-30 19:13:57
*/

#include <Cameras/Camera.hpp>

#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Halton sequence scaled to [-1:1]
const glm::vec2 haltonSequence[256] = {
    glm::vec2(0, -0.333333),
    glm::vec2(-0.5, 0.333333),
    glm::vec2(0.5, -0.777778),
    glm::vec2(-0.75, -0.111111),
    glm::vec2(0.25, 0.555556),
    glm::vec2(-0.25, -0.555556),
    glm::vec2(0.75, 0.111111),
    glm::vec2(-0.875, 0.777778),
    glm::vec2(0.125, -0.925926),
    glm::vec2(-0.375, -0.259259),
    glm::vec2(0.625, 0.407407),
    glm::vec2(-0.625, -0.703704),
    glm::vec2(0.375, -0.037037),
    glm::vec2(-0.125, 0.62963),
    glm::vec2(0.875, -0.481481),
    glm::vec2(-0.9375, 0.185185),
    glm::vec2(0.0625, 0.851852),
    glm::vec2(-0.4375, -0.851852),
    glm::vec2(0.5625, -0.185185),
    glm::vec2(-0.6875, 0.481482),
    glm::vec2(0.3125, -0.62963),
    glm::vec2(-0.1875, 0.037037),
    glm::vec2(0.8125, 0.703704),
    glm::vec2(-0.8125, -0.407407),
    glm::vec2(0.1875, 0.259259),
    glm::vec2(-0.3125, 0.925926),
    glm::vec2(0.6875, -0.975309),
    glm::vec2(-0.5625, -0.308642),
    glm::vec2(0.4375, 0.358025),
    glm::vec2(-0.0625, -0.753086),
    glm::vec2(0.9375, -0.0864198),
    glm::vec2(-0.96875, 0.580247),
    glm::vec2(0.03125, -0.530864),
    glm::vec2(-0.46875, 0.135803),
    glm::vec2(0.53125, 0.802469),
    glm::vec2(-0.71875, -0.901235),
    glm::vec2(0.28125, -0.234568),
    glm::vec2(-0.21875, 0.432099),
    glm::vec2(0.78125, -0.679012),
    glm::vec2(-0.84375, -0.0123457),
    glm::vec2(0.15625, 0.654321),
    glm::vec2(-0.34375, -0.45679),
    glm::vec2(0.65625, 0.209877),
    glm::vec2(-0.59375, 0.876543),
    glm::vec2(0.40625, -0.82716),
    glm::vec2(-0.09375, -0.160494),
    glm::vec2(0.90625, 0.506173),
    glm::vec2(-0.90625, -0.604938),
    glm::vec2(0.09375, 0.0617284),
    glm::vec2(-0.40625, 0.728395),
    glm::vec2(0.59375, -0.382716),
    glm::vec2(-0.65625, 0.283951),
    glm::vec2(0.34375, 0.950617),
    glm::vec2(-0.15625, -0.950617),
    glm::vec2(0.84375, -0.283951),
    glm::vec2(-0.78125, 0.382716),
    glm::vec2(0.21875, -0.728395),
    glm::vec2(-0.28125, -0.0617284),
    glm::vec2(0.71875, 0.604938),
    glm::vec2(-0.53125, -0.506173),
    glm::vec2(0.46875, 0.160494),
    glm::vec2(-0.03125, 0.82716),
    glm::vec2(0.96875, -0.876543),
    glm::vec2(-0.984375, -0.209877),
    glm::vec2(0.015625, 0.45679),
    glm::vec2(-0.484375, -0.654321),
    glm::vec2(0.515625, 0.0123457),
    glm::vec2(-0.734375, 0.679012),
    glm::vec2(0.265625, -0.432099),
    glm::vec2(-0.234375, 0.234568),
    glm::vec2(0.765625, 0.901235),
    glm::vec2(-0.859375, -0.802469),
    glm::vec2(0.140625, -0.135803),
    glm::vec2(-0.359375, 0.530864),
    glm::vec2(0.640625, -0.580247),
    glm::vec2(-0.609375, 0.0864197),
    glm::vec2(0.390625, 0.753086),
    glm::vec2(-0.109375, -0.358025),
    glm::vec2(0.890625, 0.308642),
    glm::vec2(-0.921875, 0.975309),
    glm::vec2(0.078125, -0.99177),
    glm::vec2(-0.421875, -0.325103),
    glm::vec2(0.578125, 0.341564),
    glm::vec2(-0.671875, -0.769547),
    glm::vec2(0.328125, -0.102881),
    glm::vec2(-0.171875, 0.563786),
    glm::vec2(0.828125, -0.547325),
    glm::vec2(-0.796875, 0.119342),
    glm::vec2(0.203125, 0.786008),
    glm::vec2(-0.296875, -0.917695),
    glm::vec2(0.703125, -0.251029),
    glm::vec2(-0.546875, 0.415638),
    glm::vec2(0.453125, -0.695473),
    glm::vec2(-0.046875, -0.0288066),
    glm::vec2(0.953125, 0.63786),
    glm::vec2(-0.953125, -0.473251),
    glm::vec2(0.046875, 0.193416),
    glm::vec2(-0.453125, 0.860082),
    glm::vec2(0.546875, -0.843621),
    glm::vec2(-0.703125, -0.176955),
    glm::vec2(0.296875, 0.489712),
    glm::vec2(-0.203125, -0.621399),
    glm::vec2(0.796875, 0.0452675),
    glm::vec2(-0.828125, 0.711934),
    glm::vec2(0.171875, -0.399177),
    glm::vec2(-0.328125, 0.26749),
    glm::vec2(0.671875, 0.934156),
    glm::vec2(-0.578125, -0.967078),
    glm::vec2(0.421875, -0.300412),
    glm::vec2(-0.078125, 0.366255),
    glm::vec2(0.921875, -0.744856),
    glm::vec2(-0.890625, -0.0781893),
    glm::vec2(0.109375, 0.588477),
    glm::vec2(-0.390625, -0.522634),
    glm::vec2(0.609375, 0.144033),
    glm::vec2(-0.640625, 0.8107),
    glm::vec2(0.359375, -0.893004),
    glm::vec2(-0.140625, -0.226337),
    glm::vec2(0.859375, 0.440329),
    glm::vec2(-0.765625, -0.670782),
    glm::vec2(0.234375, -0.00411522),
    glm::vec2(-0.265625, 0.662551),
    glm::vec2(0.734375, -0.44856),
    glm::vec2(-0.515625, 0.218107),
    glm::vec2(0.484375, 0.884774),
    glm::vec2(-0.015625, -0.81893),
    glm::vec2(0.984375, -0.152263),
    glm::vec2(-0.992188, 0.514403),
    glm::vec2(0.0078125, -0.596708),
    glm::vec2(-0.492188, 0.0699588),
    glm::vec2(0.507812, 0.736626),
    glm::vec2(-0.742188, -0.374486),
    glm::vec2(0.257812, 0.292181),
    glm::vec2(-0.242188, 0.958848),
    glm::vec2(0.757812, -0.942387),
    glm::vec2(-0.867188, -0.27572),
    glm::vec2(0.132812, 0.390947),
    glm::vec2(-0.367188, -0.720165),
    glm::vec2(0.632812, -0.0534979),
    glm::vec2(-0.617188, 0.613169),
    glm::vec2(0.382812, -0.497942),
    glm::vec2(-0.117188, 0.168724),
    glm::vec2(0.882812, 0.835391),
    glm::vec2(-0.929688, -0.868313),
    glm::vec2(0.0703125, -0.201646),
    glm::vec2(-0.429688, 0.465021),
    glm::vec2(0.570312, -0.646091),
    glm::vec2(-0.679688, 0.0205761),
    glm::vec2(0.320312, 0.687243),
    glm::vec2(-0.179688, -0.423868),
    glm::vec2(0.820312, 0.242798),
    glm::vec2(-0.804688, 0.909465),
    glm::vec2(0.195312, -0.794239),
    glm::vec2(-0.304688, -0.127572),
    glm::vec2(0.695312, 0.539095),
    glm::vec2(-0.554688, -0.572016),
    glm::vec2(0.445312, 0.0946501),
    glm::vec2(-0.0546875, 0.761317),
    glm::vec2(0.945312, -0.349794),
    glm::vec2(-0.960938, 0.316872),
    glm::vec2(0.0390625, 0.983539),
    glm::vec2(-0.460938, -0.983539),
    glm::vec2(0.539062, -0.316872),
    glm::vec2(-0.710938, 0.349794),
    glm::vec2(0.289062, -0.761317),
    glm::vec2(-0.210938, -0.0946502),
    glm::vec2(0.789062, 0.572016),
    glm::vec2(-0.835938, -0.539095),
    glm::vec2(0.164062, 0.127572),
    glm::vec2(-0.335938, 0.794239),
    glm::vec2(0.664062, -0.909465),
    glm::vec2(-0.585938, -0.242798),
    glm::vec2(0.414062, 0.423868),
    glm::vec2(-0.0859375, -0.687243),
    glm::vec2(0.914062, -0.0205761),
    glm::vec2(-0.898438, 0.646091),
    glm::vec2(0.101562, -0.465021),
    glm::vec2(-0.398438, 0.201646),
    glm::vec2(0.601562, 0.868313),
    glm::vec2(-0.648438, -0.835391),
    glm::vec2(0.351562, -0.168724),
    glm::vec2(-0.148438, 0.497942),
    glm::vec2(0.851562, -0.613169),
    glm::vec2(-0.773438, 0.0534979),
    glm::vec2(0.226562, 0.720165),
    glm::vec2(-0.273438, -0.390947),
    glm::vec2(0.726562, 0.27572),
    glm::vec2(-0.523438, 0.942387),
    glm::vec2(0.476562, -0.958848),
    glm::vec2(-0.0234375, -0.292181),
    glm::vec2(0.976562, 0.374486),
    glm::vec2(-0.976562, -0.736626),
    glm::vec2(0.0234375, -0.0699589),
    glm::vec2(-0.476562, 0.596708),
    glm::vec2(0.523438, -0.514403),
    glm::vec2(-0.726562, 0.152263),
    glm::vec2(0.273438, 0.81893),
    glm::vec2(-0.226562, -0.884774),
    glm::vec2(0.773438, -0.218107),
    glm::vec2(-0.851562, 0.44856),
    glm::vec2(0.148438, -0.662551),
    glm::vec2(-0.351562, 0.00411522),
    glm::vec2(0.648438, 0.670782),
    glm::vec2(-0.601562, -0.440329),
    glm::vec2(0.398438, 0.226337),
    glm::vec2(-0.101562, 0.893004),
    glm::vec2(0.898438, -0.8107),
    glm::vec2(-0.914062, -0.144033),
    glm::vec2(0.0859375, 0.522634),
    glm::vec2(-0.414062, -0.588477),
    glm::vec2(0.585938, 0.0781893),
    glm::vec2(-0.664062, 0.744856),
    glm::vec2(0.335938, -0.366255),
    glm::vec2(-0.164062, 0.300412),
    glm::vec2(0.835938, 0.967078),
    glm::vec2(-0.789062, -0.934156),
    glm::vec2(0.210938, -0.26749),
    glm::vec2(-0.289062, 0.399177),
    glm::vec2(0.710938, -0.711934),
    glm::vec2(-0.539062, -0.0452675),
    glm::vec2(0.460938, 0.621399),
    glm::vec2(-0.0390625, -0.489712),
    glm::vec2(0.960938, 0.176955),
    glm::vec2(-0.945312, 0.843621),
    glm::vec2(0.0546875, -0.860082),
    glm::vec2(-0.445312, -0.193416),
    glm::vec2(0.554688, 0.473251),
    glm::vec2(-0.695312, -0.63786),
    glm::vec2(0.304688, 0.0288066),
    glm::vec2(-0.195312, 0.695473),
    glm::vec2(0.804688, -0.415638),
    glm::vec2(-0.820312, 0.251029),
    glm::vec2(0.179688, 0.917695),
    glm::vec2(-0.320312, -0.786008),
    glm::vec2(0.679688, -0.119342),
    glm::vec2(-0.570312, 0.547325),
    glm::vec2(0.429688, -0.563786),
    glm::vec2(-0.0703125, 0.102881),
    glm::vec2(0.929688, 0.769547),
    glm::vec2(-0.882812, -0.341564),
    glm::vec2(0.117188, 0.325103),
    glm::vec2(-0.382812, 0.99177),
    glm::vec2(0.617188, -0.997257),
    glm::vec2(-0.632812, -0.33059),
    glm::vec2(0.367188, 0.336077),
    glm::vec2(-0.132812, -0.775034),
    glm::vec2(0.867188, -0.108368),
    glm::vec2(-0.757812, 0.558299),
    glm::vec2(0.242188, -0.552812),
    glm::vec2(-0.257812, 0.113855),
    glm::vec2(0.742188, 0.780521),
    glm::vec2(-0.507812, -0.923182),
    glm::vec2(0.492188, -0.256516),
    glm::vec2(-0.0078125, 0.410151),
    glm::vec2(0.992188, -0.70096),
    glm::vec2(-0.996094, -0.0342935)
};

static inline auto ApplyTemporalJitter(glm::mat4 projMat)
{
    static uint8_t frameNbr { 0 };
    auto& halton { haltonSequence[frameNbr] * 0.25f };
    ++frameNbr;
    projMat[2][0] += halton.x * 0.0001; // 1 / 1024.f
    projMat[2][1] += halton.y * 0.0001;
    return projMat;
}

namespace TabGraph::Cameras {
Camera::Projection::Projection(PerspectiveInfinite data)
    : type(Type::PerspectiveInfinite)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<PerspectiveInfinite>() };
        return ApplyTemporalJitter(glm::infinitePerspective(glm::radians(projData.fov), projData.aspectRatio, projData.znear));
    })
{
}

Camera::Projection::Projection(Perspective data)
    : type(Type::Perspective)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<Perspective>() };
        return ApplyTemporalJitter(glm::perspective(glm::radians(projData.fov), projData.aspectRatio, projData.znear, projData.zfar));
    })
{
}

Camera::Projection::Projection(Orthographic data)
    : type(Type::Orthographic)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<Orthographic>() };
        return ApplyTemporalJitter(glm::ortho(projData.xmag, projData.xmag, projData.ymag, projData.ymag, projData.znear, projData.zfar));
    })
{
}

Camera::Camera(const std::string& name, Camera::Projection proj)
    : Inherit(name)
    , _projection(proj)
{
}

std::array<glm::vec3, 8> Camera::ExtractFrustum()
{
    static std::array<glm::vec3, 8> NDCCube {
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f)
    };
    auto invVP = glm::inverse(GetProjection() * GetViewMatrix());
    for (auto& v : NDCCube) {
        glm::vec4 normalizedCoord = invVP * glm::vec4(v, 1);
        v = glm::vec3(normalizedCoord) / normalizedCoord.w;
    }
    return NDCCube;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::inverse(GetWorldTransformMatrix());
}

const Camera::Projection& Camera::GetProjection() const
{
    return _projection;
}

void Camera::SetProjection(const Camera::Projection& proj)
{
    _projection = proj;
}

}
