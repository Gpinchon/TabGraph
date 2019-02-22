/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 18:54:00 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

enum CameraProjection {
    OrthoCamera,
    PerspectiveCamera
};

class Camera : public Node {
public:
    static std::shared_ptr<Camera> create(const std::string&, float fov, CameraProjection proj = PerspectiveCamera);
    static std::shared_ptr<Camera> get_by_name(const std::string&);
    static std::shared_ptr<Camera> get(unsigned index);
    static std::shared_ptr<Camera> current();
    static void set_current(std::shared_ptr<Camera>);
    virtual void transform_update();
    virtual MAT4& view();
    virtual MAT4& projection();
    virtual FRUSTUM& frustum();
    virtual float& fov();

protected:
    Camera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);
    CameraProjection _projection_type { PerspectiveCamera };
    MAT4 _view { { 0 } };
    MAT4 _projection { { 0 } };
    FRUSTUM _frustum { -50, 50, -50, 50 };
    float _fov { 45 };
    float _znear { 0.1 };
    float _zfar { 1000 };
    static std::vector<std::shared_ptr<Camera>> _cameras;
    static std::weak_ptr<Camera> _current;
};

class OrbitCamera : public Camera {
public:
    static std::shared_ptr<OrbitCamera> create(const std::string&, float fov, float phi, float theta, float radius);
    //virtual void	update();
    void orbite(float phi, float theta, float radius);

private:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius);
    float _phi;
    float _theta;
    float _radius;
};
