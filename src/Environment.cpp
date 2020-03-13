/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:03
*/

#include "Environment.hpp"
#include "Cubemap.hpp"

std::weak_ptr<Environment> Environment::_current;
std::vector<std::shared_ptr<Environment>> Environment::_environments;

Environment::Environment(const std::string& name)
    : Object(name)
{
}

std::shared_ptr<Environment> Environment::Create(const std::string& name)
{
    auto e = std::shared_ptr<Environment>(new Environment(name));
    _environments.push_back(e);
    return (e);
}

std::shared_ptr<Environment> Environment::Get(unsigned index)
{
    if (index >= _environments.size())
        return (nullptr);
    return (_environments.at(index));
}

std::shared_ptr<Environment> Environment::current()
{
    return (_current.lock());
}

void Environment::set_current(std::shared_ptr<Environment> cur)
{
    _current = cur;
}

std::shared_ptr<Cubemap> Environment::diffuse()
{
    return (_diffuse);
}

void Environment::set_diffuse(std::shared_ptr<Cubemap> dif)
{
    _diffuse = dif;
}

std::shared_ptr<Cubemap> Environment::irradiance()
{
    return (_irradiance);
}

void Environment::set_irradiance(std::shared_ptr<Cubemap> irr)
{
    _irradiance = irr;
}

void Environment::unload()
{
    diffuse()->unload();
    irradiance()->unload();
}