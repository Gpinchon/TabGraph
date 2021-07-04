/*
* @Author: gpinchon
* @Date:   2021-06-27 17:52:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:44
*/

#pragma once

#include <Nodes/Node.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Shape;
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Renderable : public Core::Inherit<Node, Renderable> {
public:
    inline void Add(std::shared_ptr<Shapes::Shape> shape)
    {
        _shapes.insert(shape);
    }
    inline void Remove(std::shared_ptr<Shapes::Shape> shape)
    {
        _shapes.erase(shape);
    }
    inline void ClearShapes()
    {
        _shapes.clear();
    }
    inline auto& GetShapes() const
    {
        return _shapes;
    }
    inline auto GetShapeNbr() const
    {
        return _shapes.size();
    }

private:
    std::set<std::shared_ptr<Shapes::Shape>> _shapes;
};
};
