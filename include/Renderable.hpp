/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-08-13 17:16:51
*/

#pragma once

#include "Node.hpp" // for Node
#include <memory> // for shared_ptr
#include <string> // for string
#include <vector> // for vector

enum class RenderMod
{
    RenderAll,
    RenderOpaque,
    RenderTransparent
};

class Renderable : public Node
{
public:
    static std::shared_ptr<Renderable> Get(unsigned index);
    static std::shared_ptr<Renderable> GetByName(const std::string& name);
    virtual bool Draw(RenderMod mod = RenderMod::RenderAll) = 0;
    virtual bool DrawDepth(RenderMod mod = RenderMod::RenderAll) = 0;
    virtual bool Drawable() const = 0;
    virtual void Load() = 0;
    bool is_loaded();

protected:
    bool _is_loaded{false};
    Renderable(const std::string &name);

private:
};
