/*
* @Author: gpi
* @Date:   2019-07-16 08:53:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:09
*/

#include <Camera.hpp>

class OrbitCamera : public Camera {
public:
    static std::shared_ptr<OrbitCamera> Create(const std::string&, float fov, float phi, float theta, float radius, CameraProjection proj = PerspectiveCamera);
    virtual void UpdateViewMatrix() override;
    float Phi() const;
    void SetPhi(float);
    float Theta() const;
    void SetTheta(float);
    float Radius() const;
    void SetRadius(float);

private:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius, CameraProjection proj);
    float _phi { 0 };
    float _theta { 0 };
    float _radius { 0 };
};
