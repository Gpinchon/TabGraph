/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:00
*/
#pragma once

#include "Light/Light.hpp"

class Framebuffer;
namespace Shader {
class Program;
};

class DirectionnalLight : public Light {
    PROPERTY(bool, Infinite, true);
    READONLYPROPERTY(glm::vec3, Direction, glm::vec3(0, 1, 0));

public:
    DirectionnalLight();
    DirectionnalLight(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow = false);
    void SetDirection(const glm::vec3&);
    glm::vec3 GetHalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 GetMin() const;
    glm::vec3 GetMax() const;
    virtual void render_shadow() override;
    virtual void Draw() override;
    virtual void DrawProbe(LightProbe& lightProbe) override;

protected:
    void DrawShadowInfinite();
    void DrawShadowFinite();
    glm::mat4 ShadowProjectionMatrixInfinite() const;
    glm::mat4 ShadowProjectionMatrixFinite() const;
    glm::mat4 ShadowViewMatrix() const;
    std::shared_ptr<Framebuffer> _GetShadowBuffer() const;
    void _SetShadowBuffer(std::shared_ptr<Framebuffer> buffer);

    bool _needsUpdate { true };
    //std::shared_ptr<Cubemap> _specularLUT;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _shadowBuffer;
};
