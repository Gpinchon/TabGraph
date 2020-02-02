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

class Camera : public Node {
public:
    static std::shared_ptr<Camera> Create(const std::string&, float fov, CameraProjection proj = PerspectiveCamera);
    static std::shared_ptr<Camera> Create(std::shared_ptr<Camera> otherCamera);
    static std::shared_ptr<Camera> GetByName(const std::string&);
    static std::shared_ptr<Camera> GetById(int64_t id);
    static std::shared_ptr<Camera> current();
    static void set_current(std::shared_ptr<Camera>);
    /** Overload this to change Camera's behavior */
    virtual void UpdateViewMatrix();
    /** Overload this to change Camera's behavior */
    virtual void UpdateProjectionMatrix();
    virtual glm::mat4 ViewMatrix() const;
    virtual void SetViewMatrix(glm::mat4);
    virtual glm::mat4 ProjectionMatrix() const;
    virtual void SetProjectionMatrix(glm::mat4);
    virtual glm::ivec4& frustum();
    virtual float Fov() const;
    virtual void SetFov(float);
    /** Alias for Rotation */
    virtual glm::vec3 Forward() const;
    /** Alias for SetRotation */
    virtual void SetForward(glm::vec3);
    /** READONLY : Computed on demand */
    virtual glm::vec3 Up() const;
    /** READONLY : Computed on demand */
    virtual glm::vec3 Right() const;
    virtual float Znear() const;
    virtual void SetZnear(float);
    virtual float Zfar() const;
    virtual void SetZfar(float);
    virtual ~Camera() = default;

protected:
    Camera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);

private:
    /** Calls UpdateViewMatrix and UpdateProjectionMatrix */
    virtual void UpdateTransformMatrix() final override;
    CameraProjection _projection_type { PerspectiveCamera };
    static std::weak_ptr<Camera> _current;
    glm::mat4 _projection { 0 };
    glm::ivec4 _frustum { -50, 50, -50, 50 };
    float _fov { 45 };
    float _znear { 0.1 };
    float _zfar { 1000 };
};
