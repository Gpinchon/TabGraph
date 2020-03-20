/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:33:45
*/

#pragma once

#include "Common.hpp"
#include "Node.hpp" // for Node
#include <glm/glm.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

enum CameraProjection {
    OrthoCamera,
    PerspectiveCamera
};

/**
 * @brief The default "general purpose" camera
 */
class Camera : public Node {
public:
    static std::shared_ptr<Camera> Create(const std::string&, float fov, CameraProjection proj = PerspectiveCamera);
    static std::shared_ptr<Camera> Create(std::shared_ptr<Camera> otherCamera);
    /** Overload this to change Camera's behavior */
    //virtual void UpdateProjectionMatrix();
    /**
     * @brief alias for TransformMatrix
     * @return the camera's view matrix
     */
    glm::mat4 ViewMatrix() const;
    /**
     * @brief alias for SetTransformMatrix
     * @arg viewMatrix : the new camera's view matrix
     */
    void SetViewMatrix(glm::mat4 viewMatrix);
    /**
     * @brief READONLY : Computed on demand
     */
    virtual glm::mat4 ProjectionMatrix() const;
    //virtual void SetProjectionMatrix(glm::mat4);
    virtual glm::ivec4 Frustum() const;
    /** @return the vertical field of view in degrees */
    virtual float Fov() const;
    /** @arg fov : the vertical field of view in degrees */
    virtual void SetFov(float fov);
    /**
     * @brief Common::Forward() * Rotation()
     * READONLY : Computed on demand
     */
    virtual glm::vec3 Forward() const;
    /**
     * @brief READONLY : Computed on demand
     * @return Common::Up() * Rotation()
     */
    virtual glm::vec3 Up() const;
    /**
     * @brief READONLY : Computed on demand
     * Common::Right() * Rotation()
     */
    virtual glm::vec3 Right() const;
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
    virtual ~Camera() = default;

protected:
    Camera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);

private:
    /** Calls UpdateViewMatrix and UpdateProjectionMatrix */
    void UpdateTransformMatrix() final override;
    glm::mat4 TransformMatrix() const final override;
    void SetTransformMatrix(glm::mat4) final override;
    CameraProjection _projection_type { PerspectiveCamera };
    //glm::mat4 _projection { 0 };
    glm::ivec4 _frustum { -50, 50, -50, 50 };
    float _fov { 45 };
    float _znear { 0.1 };
    float _zfar { 0 };
};
