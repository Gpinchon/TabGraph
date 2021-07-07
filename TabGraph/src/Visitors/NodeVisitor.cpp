/*
* @Author: gpinchon
* @Date:   2021-06-20 13:28:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:28:58
*/

#include <Visitors/NodeVisitor.hpp>
#include <Nodes/Node.hpp>
#include <Nodes/Group.hpp>
#include <Nodes/Renderable.hpp>

namespace TabGraph::Visitors {
void NodeVisitor::_Traverse(Nodes::Node& node)
{
	switch (mode)
	{
	case Mode::VisitOnce:
		return;
	case Mode::VisitParents:
		node.GetParent()->Accept(*this);
		break;
	default:
		break;
	}
}

void NodeVisitor::_Traverse(Nodes::Group& group)
{
	if (mode == Mode::VisitChildren)
		for (auto& child : group.GetChildren()) child->Accept(*this);
	else _Traverse(static_cast<Nodes::Node&>(group));
}
void NodeVisitor::_Traverse(Nodes::Renderable& renderable)
{
	_Traverse(static_cast<Nodes::Node&>(renderable));
}
}
