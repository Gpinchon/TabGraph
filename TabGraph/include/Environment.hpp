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

class Environment : public Component {
public:
    Environment(const std::string& name);
    void unload();
    std::shared_ptr<Cubemap> diffuse();
    void set_diffuse(std::shared_ptr<Cubemap>);
    std::shared_ptr<Cubemap> irradiance();
    void set_irradiance(std::shared_ptr<Cubemap>);

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
    std::shared_ptr<Cubemap> _diffuse;
    std::shared_ptr<Cubemap> _irradiance;
};
