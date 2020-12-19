/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:08
*/

#pragma once

#include "Component.hpp" // for Object
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Cubemap;
class Shader;

class Environment : public Component {
    PROPERTY(std::shared_ptr<Shader>, Shader, nullptr);
    PROPERTY(std::shared_ptr<Cubemap>, Diffuse, nullptr);
    PROPERTY(std::shared_ptr<Cubemap>, Irradiance, nullptr);
public:
    Environment(const std::string& name);
    void unload();
    void Draw();

protected:
    virtual std::shared_ptr<Component> _Clone() override {
        auto env(Component::Create<Environment>(*this));
        return env;
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
};
