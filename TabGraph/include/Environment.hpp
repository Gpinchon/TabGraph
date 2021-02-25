/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:44
*/

#pragma once

#include "Component.hpp" // for Object
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Cubemap;
namespace Shader {
    class Program;
}

class Environment : public Component {
    PROPERTY(std::shared_ptr<Shader::Program>, Shader, nullptr);
    PROPERTY(std::shared_ptr<Cubemap>, Diffuse, nullptr);
    PROPERTY(std::shared_ptr<Cubemap>, Irradiance, nullptr);

public:
    Environment(const std::string& name);
    void unload();
    void Draw();

protected:
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto env(Component::Create<Environment>(*this));
        return env;
    }
};
