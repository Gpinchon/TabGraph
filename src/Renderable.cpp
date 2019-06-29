/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:54:38
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

std::shared_ptr<Renderable> Renderable::get_by_name(const std::string& name)
{
    for (auto r : _renderables) {
        if (name == r->name())
            return (r);
    }
    return (nullptr);
}

bool Renderable::is_loaded()
{
    return (_is_loaded);
}

void Renderable::add(std::shared_ptr<Renderable> renderable)
{
    _renderables.push_back(renderable);
}
