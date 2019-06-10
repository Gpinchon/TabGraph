/*
* @Author: gpi
* @Date:   2019-06-07 13:35:27
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-07 14:22:21
*/

#pragma once

#include <array>
#include <vector>
#include <vml.h>

template<typename T>
class Quadtree
{
public:
	Quadtree(T data, VEC2 min, VEC2 max) : _data(data), _min(min), _max(max) {};
	Quadtree() = delete;
	~Quadtree();
	Quadtree<T>		*Get(int childIndex);
	bool			Insert(T data, VEC2 min, VEC2 max);
	std::vector<T>	&Data();

private:
	std::array<Quadtree<T> *, 4>	_children;
	std::vector<T>	_data;
	VEC2	_min {0, 0};
	VEC2	_max {0, 0};
	VEC2	_size{0, 0};
};

template<typename T>
inline bool				Quadtree<T>::Insert(T data, VEC2 min, VEC2 max)
{
	int		index = -1;
	auto	pos = new_vec2(_max.x - _size.x / 2.f, _max.y - _size.y / 2.f);
	bool	top = false;
	bool	bottom = false;
	bool	left = false;
	bool	right = false;
	bool	inside = min.x >= _min.x && min.y >= _min.y
			&&	max.x <= _max.x && max.y <= _max.y;
	auto	childMin = new_vec2(-1, -1);
	auto	childMax = new_vec2(-1, -1);
	if (!inside)
		return false;
	if (min.y > pos.y) {
		top = true;
		childMin.y = pos.y;
		childMax.y = _max.y;
	}
	else if (max.y < pos.y) {
		bottom = true;
		childMin.y = _min.y;
		childMax.y = pos.y;
	}
	if (max.x < pos.x) {
		left = true;
		childMin.x = _min.x;
		childMax.x = pos.x;
	}
	else if (min.x > pos.x) {
		right = true;
		childMin.x = pos.x;
		childMax.x = _max.x;
	}

	if (top) {
		if (left)
			index = 0;
		else if (right)
			index = 1;
	}
	else if (bottom) {
		if (left)
			index = 2;
		else if (right)
			index = 3;
	}
	if (index == -1) {
		_data.push_back(data);
		return true;
	}
	_children.at(index) = new Quadtree<T>(data, childMin, childMax);
	return _children.at(index)->insert(data, min, max);

}

template<typename T>
inline Quadtree<T>		*Quadtree<T>::Get(int childIndex)
{
	return _children.at(childIndex);
}

template<typename T>
inline std::vector<T>	&Quadtree<T>::Data()
{
	return _data;
}