#pragma once

#include "BoundingElement.hpp"
#include <memory>

class Mesh;

class BoundingMesh : public BoundingElement
{
public:
	BoundingMesh(std::shared_ptr<::Mesh> mesh = nullptr);
	std::shared_ptr<::Mesh> GetMesh() const;
	void SetMesh(std::shared_ptr<::Mesh> mesh);

private:
	std::shared_ptr<::Mesh> _mesh { nullptr };
	
};