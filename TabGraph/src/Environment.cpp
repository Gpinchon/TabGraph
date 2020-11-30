/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 20:17:28
*/

#include "Environment.hpp"
#include "Texture/Cubemap.hpp"

Environment::Environment(const std::string& name)
    : Component(name)
{
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