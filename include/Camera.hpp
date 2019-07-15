/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 16:51:03
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
    static std::shared_ptr<Camera> create(const std::string&, float fov, CameraProjection proj = PerspectiveCamera);
    static std::shared_ptr<Camera> get_by_name(const std::string&);
    static std::shared_ptr<Camera> Get(unsigned index);
    static std::shared_ptr<Camera> current();
    static void set_current(std::shared_ptr<Camera>);
    static void add(std::shared_ptr<Camera>);
    virtual void UpdateViewMatrix();
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
    virtual ~Camera() = default;

protected:
    Camera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);

private:
    virtual void UpdateTransformMatrix() final override;
    CameraProjection _projection_type { PerspectiveCamera };
    static std::vector<std::shared_ptr<Camera>> _cameras;
    static std::weak_ptr<Camera> _current;
    glm::mat4 _projection { 0 };
    glm::ivec4 _frustum { -50, 50, -50, 50 };
    float _fov { 45 };
    float _znear { 0.1 };
    float _zfar { 1000 };
};

class OrbitCamera : public Camera {
public:
    static std::shared_ptr<OrbitCamera> create(const std::string&, float fov, float phi, float theta, float radius);
    //virtual void	Update();
    void orbite(float phi, float theta, float radius);

private:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius);
    float _phi;
    float _theta;
    float _radius;
};
