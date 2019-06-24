/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:54:51
*/

#pragma once

#include <memory>    // for shared_ptr
#include <string>    // for string
#include <vector>    // for vector
#include "Node.hpp"  // for Node

enum RenderMod {
    RenderAll,
    RenderOpaque,
    RenderTransparent
};

class Renderable : public Node {
public:
    static std::shared_ptr<Renderable> Get(unsigned index);
    static std::shared_ptr<Renderable> get_by_name(const std::string&);
    virtual bool render(RenderMod mod = RenderAll) = 0;
    virtual bool render_depth(RenderMod mod = RenderAll) = 0;
    virtual void load() = 0;
    bool is_loaded();

protected:
    bool _is_loaded{ false };
    static void add(std::shared_ptr<Renderable>);
    Renderable(const std::string& name);

private:
    static std::vector<std::shared_ptr<Renderable>> _renderables;
};
