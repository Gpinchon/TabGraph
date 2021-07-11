/*
* @Author: gpinchon
* @Date:   2021-06-29 22:43:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-29 22:43:55
*/

#include <Shapes\Shape.hpp>

using namespace TabGraph;

void Renderer::ShapeRendererDeleter::operator()(ShapeRenderer* p)
{
	delete p;
}

Shapes::Shape::~Shape()
{
}
