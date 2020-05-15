#pragma once

#include "BoundingElement.hpp"
#include <memory>

class Geometry;

class BoundingGeometry : public BoundingElement
{
public:
	BoundingGeometry(std::shared_ptr<::Geometry> geommetry);
	std::shared_ptr<::Geometry> GetGeometry() const;
	void SetGeometry(std::shared_ptr<::Geometry> geommetry);

private:
	std::shared_ptr<::Geometry> _mesh { nullptr };
	
};