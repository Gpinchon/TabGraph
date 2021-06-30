/*
* @Author: gpinchon
* @Date:   2021-06-29 22:43:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-29 22:43:55
*/

#include <SceneGraph\Shape.hpp>

void Renderer::ShapeRendererDeleter::operator()(ShapeRenderer* p)
{
	delete p;
}

SceneGraph::Shape::~Shape()
{
}
