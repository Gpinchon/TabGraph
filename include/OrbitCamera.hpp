/*
* @Author: gpi
* @Date:   2019-07-16 08:53:02
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-16 09:40:17
*/

#include <Camera.hpp>

class OrbitCamera : public Camera {
public:
    static std::shared_ptr<OrbitCamera> create(const std::string&, float fov, float phi, float theta, float radius, CameraProjection proj = PerspectiveCamera);
    virtual void UpdateViewMatrix() override;
    float Phi() const;
    void SetPhi(float);
    float Theta() const;
    void SetTheta(float);
    float Radius() const;
    void SetRadius(float);

private:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius, CameraProjection proj);
    float _phi {0};
    float _theta {0};
    float _radius {0};
};
