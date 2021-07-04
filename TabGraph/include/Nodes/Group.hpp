/*
* @Author: gpinchon
* @Date:   2021-07-04 16:32:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-04 16:37:49
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Nodes/Node.hpp>
#include <Core/Inherit.hpp>

#include <set>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
/**
 * @brief Describes a node group, can have children and a parent
*/
class Group : public Core::Inherit<Nodes::Node, Group> {
public:
    Group(const Group& other)
        : Inherit(other)
    {
        //nodes group cannot have the children of another group
        _children.clear();
    }
    /** @return true if @arg child is in children list */
    inline auto HasChild(std::shared_ptr<Node> child)
    {
        return _children.count(child) > 0;
    };
    /** @return the number of children */
    inline auto ChildCount() const
    {
        return _children.size();
    };
    /** @return all the children */
    inline auto& GetChildren() const
    {
        return _children;
    }
    /** removes parenting for all children */
    inline void ClearChildren()
    {
        auto children{ _children };
        for (auto& child : children)
            child->SetParent(nullptr);
        _children.clear();
    }

private:
    friend Node;
    inline void _AddChild(std::shared_ptr<Node> child)
    {
        _children.insert(child);
    }
    /** removes parenting for specified @arg child */
    inline void _RemoveChild(std::shared_ptr<Node> child)
    {
        _children.erase(child);
    }
    std::set<std::shared_ptr<Node>> _children;
};
}
