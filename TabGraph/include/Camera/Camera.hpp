/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:34
*/

#pragma once

#include "Common.hpp"
#include "Node.hpp" // for Node
#include <glm/glm.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

/**
 * @brief The default "general purpose" camera
 */
class Camera : public Node {
public:
    enum class Projection {
        Invalid = -1,
        Ortho,
        Perspective
    };
    Camera(const std::string& name, Camera::Projection proj = Projection::Perspective);
    /**
     * @brief alias for TransformMatrix
     * @return the camera's view matrix
     */
    glm::mat4 GetViewMatrix();
    void SetViewMatrix(glm::mat4 viewMatrix);
    virtual glm::mat4 GetProjectionMatrix(const glm::ivec2& resolution);
    virtual void SetProjectionMatrix(glm::mat4 projectionMatrix);

    /**
     * @brief Computes the camera frustum's 8 corners
     * @arg resolution : the resolution of the Window used to compute Projection Matrix
     * @return the camera frustum's 8 corners in world space
    */
    virtual std::array<glm::vec3, 8> ExtractFrustum(const glm::ivec2& resolution);

    //virtual void SetProjectionMatrix(glm::mat4);
    virtual glm::vec4 Frustum() const;
    virtual void SetFrustum(glm::vec4 frustum);
    /** @return the vertical field of view in degrees */
    virtual float Fov() const;
    /** @arg fov : the vertical field of view in degrees */
    virtual void SetFov(float fov);
    /** @return the near clipping plane distance */
    virtual float Znear() const;
    /** @arg znear : the new near clipping plane distance */
    virtual void SetZnear(float znear);
    /** @return the far clipping plane distance */
    virtual float Zfar() const;
    /**
     * @brief set this to 0 for infinite perspective projection
     * @arg zfar : the new far clipping plane distance
     */
    virtual void SetZfar(float zfar);
    virtual Camera::Projection ProjectionType() const;
    virtual void SetProjectionType(Camera::Projection projectionType);
    virtual ~Camera() = default;

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Camera>(*this);
    }
    Camera::Projection _projectionType { Projection::Perspective };
    glm::mat4 _projectionMatrix { 0 };
    glm::mat4 _viewMatrix{ 1 };
    glm::vec4 _frustum { -50, 50, -50, 50 };
    float _fov { 45 };
    float _znear { 0.1 };
    float _zfar { 0 };

protected:
    bool _projectionMatrixNeedsUpdate{ true };
    bool _viewMatrixNeedsUpdate{ true };
};
