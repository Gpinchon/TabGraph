#include "Intersection.hpp"

Intersection::Intersection(const bool intersects, const float distance) : _intersects(intersects), _distance(distance)
{
}

bool Intersection::GetIntersects() const
{
	return _intersects;
}

float Intersection::GetDistance() const
{
	return _distance;
}
