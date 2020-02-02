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

std::shared_ptr<Renderable> Renderable::Get(unsigned index)
{
    return std::dynamic_pointer_cast<Renderable>(Node::Get(index));
}

std::shared_ptr<Renderable> Renderable::GetByName(const std::string& name)
{
    return std::dynamic_pointer_cast<Renderable>(Node::GetByName(name));
}

bool Renderable::is_loaded()
{
    return (_is_loaded);
}
