/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:54:10
*/

#include "Renderable.hpp"

std::vector<std::shared_ptr<Renderable>> Renderable::_renderables;

Renderable::Renderable(const std::string& name)
    : Node(name)
{
}

std::shared_ptr<Renderable> Renderable::Get(unsigned index)
{
    if (index >= _renderables.size())
        return (nullptr);
    return (_renderables.at(index));
}

std::shared_ptr<Renderable> Renderable::GetByName(const std::string& name)
{
    for (auto r : _renderables) {
        if (name == r->Name())
            return (r);
    }
    return (nullptr);
}

bool Renderable::is_loaded()
{
    return (_is_loaded);
}

void Renderable::Add(std::shared_ptr<Renderable> renderable)
{
    Node::Add(renderable);
    _renderables.push_back(renderable);
}
