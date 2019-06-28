/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-28 13:09:59
*/

#pragma once

#include <memory>    // for shared_ptr, weak_ptr
#include <string>    // for string
#include <vector>    // for vector
#include "Node.hpp"  // for Node
//#include "glm"     // for glm::mat4, FRUSTUM
#include <glm/glm.hpp>

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
    virtual void transform_update() override;
    virtual glm::mat4& view();
    virtual glm::mat4& projection();
    virtual glm::ivec4& frustum();
    virtual float& fov();
    virtual ~Camera() = default;
protected:
    Camera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);
    CameraProjection _projection_type{ PerspectiveCamera };
    glm::mat4 _projection{ 0 };
    glm::ivec4 _frustum{ -50, 50, -50, 50 };
    float _fov{ 45 };
    float _znear{ 0.1 };
    float _zfar{ 1000 };
    static std::vector<std::shared_ptr<Camera>> _cameras;
    static std::weak_ptr<Camera> _current;
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
