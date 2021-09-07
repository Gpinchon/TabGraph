/*
* @Author: gpinchon
* @Date:   2021-07-21 21:53:09
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 22:04:50
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>

#include <variant>
#include <functional>

#include <glm/gtc/matrix_transform.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Cameras {
class Projection {
    PROPERTY(float, TemporalJitterIntensity, 1 / 1024.f);
public:
    enum class Type {
        PerspectiveInfinite,
        Perspective,
        Orthographic,
        MaxValue
    };
    Type type { Type::PerspectiveInfinite };
    struct PerspectiveInfinite {
        float fov { 45 };
        float aspectRatio { 16 / 9.f };
        float znear { 0.1 };
    };
    struct Perspective {
        float fov { 45 };
        float aspectRatio { 16 / 9.f };
        float znear { 0.1 };
        float zfar { 1000 };
    };
    struct Orthographic {
        float xmag { 50 };
        float ymag { 50 };
        float znear { 0.1 };
        float zfar { 1000 };
    };
    Projection()
        : Projection(PerspectiveInfinite())
    {
    }
    Projection(PerspectiveInfinite data);
    Projection(Perspective data);
    Projection(Orthographic data);
    template <typename T>
    inline auto& Get() const
    {
        return std::get<T>(_data);
    }
    inline auto GetMatrix() const
    {
        return _matrixFunctor(*this);
    }
    operator glm::mat4() const
    {
        return GetMatrix();
    }
    inline glm::mat4 operator*(const glm::mat4& other) const
    {
        return GetMatrix() * other;
    }
    inline glm::mat4 operator*(const Projection& other) const
    {
        return GetMatrix() * other.GetMatrix();
    }
    static void SetTemporalJitterIndex(uint8_t index);

private:
    std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
    std::function<glm::mat4(const Projection&)> _matrixFunctor;
    static void _ApplyTemporalJitter(glm::mat4& matrix, float intensity);
};

inline Projection::Projection(PerspectiveInfinite data)
    : type(Type::PerspectiveInfinite)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<PerspectiveInfinite>() };
        auto matrix{ glm::infinitePerspective(glm::radians(projData.fov), projData.aspectRatio, projData.znear) };
        _ApplyTemporalJitter(matrix, proj.GetTemporalJitterIntensity());
        return matrix;
    })
{
}

inline Projection::Projection(Perspective data)
    : type(Type::Perspective)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<Perspective>() };
        auto matrix{ glm::perspective(glm::radians(projData.fov), projData.aspectRatio, projData.znear, projData.zfar) };
        _ApplyTemporalJitter(matrix, proj.GetTemporalJitterIntensity());
        return matrix;
    })
{
}

inline Projection::Projection(Orthographic data)
    : type(Type::Orthographic)
    , _data(data)
    , _matrixFunctor([](const Projection& proj) {
        const auto& projData { proj.Get<Orthographic>() };
        auto matrix{ glm::ortho(projData.xmag, projData.xmag, projData.ymag, projData.ymag, projData.znear, projData.zfar) };
        _ApplyTemporalJitter(matrix, proj.GetTemporalJitterIntensity());
        return matrix;
    })
{
}

}