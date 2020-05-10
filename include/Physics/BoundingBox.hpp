#pragma once

#include "BoundingElement.hpp"

class BoundingBox : public BoundingElement
{
public:
	glm::vec3 Min(const glm::mat4 &transformMatrix) const;
    glm::vec3 Max(const glm::mat4 &transformMatrix) const;
    glm::vec3 Size() const;
    void SetSize(glm::vec3 size);

private:
	glm::vec3 _size { 0 };

};