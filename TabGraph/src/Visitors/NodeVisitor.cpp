/*
* @Author: gpinchon
* @Date:   2021-06-20 13:28:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:28:58
*/

#include <Visitors/NodeVisitor.hpp>
#include <Nodes/Node.hpp>

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
	case Mode::VisitChildren:
		for (auto &child : node.GetChildren()) child->Accept(*this);
		break;
	default:
		break;
	}
}
}
