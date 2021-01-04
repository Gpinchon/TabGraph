#pragma once

#include "Light/Light.hpp"

class DirectionnalLight : public Light {
public:
    DirectionnalLight();
    DirectionnalLight(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow = false);
    glm::vec3 Direction() const;
    void SetDirection(const glm::vec3&);
    glm::vec3 HalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 Min() const;
    glm::vec3 Max() const;
    bool Infinite() const;
    void SetInfinite(bool infinite);
    virtual void render_shadow() override;
    virtual void Draw() override;

private:
    void DrawShadowInfinite();
    void DrawShadowFinite();
    glm::mat4 ShadowProjectionMatrixInfinite() const;
    glm::mat4 ShadowProjectionMatrixFinite() const;
    glm::mat4 ShadowViewMatrix() const;
    glm::vec3 _direction{ 0, 1, 0 };
    bool _infinite{ true };
};
