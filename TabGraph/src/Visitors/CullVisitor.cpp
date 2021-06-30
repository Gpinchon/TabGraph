/*
* @Author: gpinchon
* @Date:   2021-06-20 13:29:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:29:18
*/

#include <Visitors/CullVisitor.hpp>
#include <Nodes/GeoNode.hpp>

namespace TabGraph::Visitors {
	void CullVisitor::operator()(Nodes::Renderable& geoNode)
	{
		for (const auto& surface : geoNode.GetSurfaces()) {
			Renderer::ShapeState state;
			state.surface = surface;
			state.transform = geoNode.GetWorldTransformMatrix();
			_result.push_back(state);
		}
	}
}

