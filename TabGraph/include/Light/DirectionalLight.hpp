/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:00
*/
#pragma once

#include "Light/Light.hpp"

class DirectionalLight : public Light {
    PROPERTY(bool, Infinite, true);
    READONLYPROPERTY(glm::vec3, Direction, glm::vec3(0, 1, 0));

public:
    DirectionalLight();
    DirectionalLight(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow = false);
    void SetDirection(const glm::vec3&);
    glm::vec3 GetHalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 GetMin() const;
    glm::vec3 GetMax() const;
};
