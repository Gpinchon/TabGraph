/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-14 19:07:10
*/
#pragma once

#include "Light/Light.hpp"

class Texture2D;
class Framebuffer;
namespace Shader {
    class Program;
};

class DirectionnalLight : public Light {
public:
    DirectionnalLight();
    DirectionnalLight(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow = false);
    glm::vec3 GetDirection() const;
    void SetDirection(const glm::vec3&);
    glm::vec3 HalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 Min() const;
    glm::vec3 Max() const;
    bool Infinite() const;
    void SetInfinite(bool infinite);
    virtual void render_shadow() override;
    virtual void Draw() override;

protected:
    void DrawShadowInfinite();
    void DrawShadowFinite();
    glm::mat4 ShadowProjectionMatrixInfinite() const;
    glm::mat4 ShadowProjectionMatrixFinite() const;
    glm::mat4 ShadowViewMatrix() const;
    std::shared_ptr<Shader::Program> _GetShader() const;
    void _SetShader(std::shared_ptr<Shader::Program> shader);
    std::shared_ptr<Framebuffer> _GetShadowBuffer() const;
    void _SetShadowBuffer(std::shared_ptr<Framebuffer> buffer);

private:
    glm::vec3 _direction { 0, 1, 0 };
    bool _infinite { true };
    bool _diffuseLUTNeedsUpdate { true };
    std::shared_ptr<Texture2D> _specularLUT;
    std::shared_ptr<Shader::Program> _shader;
    std::shared_ptr<Framebuffer> _shadowBuffer;
};
