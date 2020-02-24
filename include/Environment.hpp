/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:08
*/

#pragma once

#include "Object.hpp" // for Object
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Cubemap;

class Environment : public Object {
public:
    static std::shared_ptr<Environment> Create(const std::string& name);
    static std::shared_ptr<Environment> Get(unsigned index);
    static std::shared_ptr<Environment> current();
    static void set_current(std::shared_ptr<Environment>);
    void unload();
    std::shared_ptr<Cubemap> diffuse();
    void set_diffuse(std::shared_ptr<Cubemap>);
    std::shared_ptr<Cubemap> irradiance();
    void set_irradiance(std::shared_ptr<Cubemap>);

protected:
    static std::vector<std::shared_ptr<Environment>> _environments;
    static std::weak_ptr<Environment> _current;
    std::weak_ptr<Cubemap> _diffuse;
    std::weak_ptr<Cubemap> _irradiance;
    Environment(const std::string& name);
};
