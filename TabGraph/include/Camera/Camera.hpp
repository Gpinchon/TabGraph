/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:34
*/

#pragma once

#include "Common.hpp"
#include "Node.hpp" // for Node

#include <glm/mat4x4.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector
#include <variant>

/**
 * @brief The default "general purpose" camera
 */
class Camera : public Node {
public:
    struct Projection {
        enum class Type {
            PerspectiveInfinite,
            Perspective,
            Orthographic,
            MaxValue
        };
        Type type{ Type::PerspectiveInfinite };
        struct PerspectiveInfinite {
            float fov{ 45 };
            float aspectRatio{ 16 / 9.f };
            float znear{ 0.1 };
        };
        struct Perspective {
            float fov{ 45 };
            float aspectRatio{ 16 / 9.f };
            float znear{ 0.1 };
            float zfar{ 1000 };
        };
        struct Orthographic {
            float xmag{ 50 };
            float ymag{ 50 };
            float znear{ 0.1 };
            float zfar{ 1000 };
        };
        Projection() : Projection(PerspectiveInfinite()){}
        Projection(PerspectiveInfinite data);
        Projection(Perspective data);
        Projection(Orthographic data);
        template<typename T>
        inline auto &Get() const {
            return std::get<T>(_data);
        }
        inline auto GetMatrix() const {
            return _matrixFunctor(*this);
        }
        operator glm::mat4() const {
            return GetMatrix();
        }
        inline glm::mat4 operator*(const glm::mat4& other) const {
            return GetMatrix() * other;
        }
        inline glm::mat4 operator*(const Projection& other) const {
            return GetMatrix() * other.GetMatrix();
        }

    private:
        std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
        std::function<glm::mat4(const Projection&)> _matrixFunctor;
    };
    Camera(const std::string& name, Camera::Projection = Camera::Projection::PerspectiveInfinite());
    ~Camera() = default;

    /**
     * @brief alias for inverse TransformMatrix
     * @return the camera's view matrix
     */
    glm::mat4 GetViewMatrix();
    /**
     * @brief Computes the camera frustum's 8 corners
     * @return the camera frustum's 8 corners in world space
    */
    std::array<glm::vec3, 8> ExtractFrustum();

    const Camera::Projection &GetProjection() const;
    void SetProjection(const Camera::Projection& projectionType);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Camera>(*this);
    }
    Projection _projection;
};
