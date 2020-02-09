/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:54:10
*/

#include "Renderable.hpp"

Renderable::Renderable(const std::string& name)
    : Node(name)
{
}

bool Renderable::is_loaded()
{
    return (_is_loaded);
}
