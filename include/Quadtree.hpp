/*
* @Author: gpi
* @Date:   2019-06-07 13:35:27
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-12 15:17:19
*/

#pragma once

#include <array>
#include <vector>
#include <vml.h>

#include <iostream>

template<typename T>
class Quadtree
{
public:
	Quadtree(VEC2 min, VEC2 max, int maxLevel = -1) : _min(min), _max(max), _mid(new_vec2((max.x + min.x) / 2.f, (max.y + min.y) / 2.f)), _maxLevel(maxLevel) {};
	Quadtree() = delete;
	~Quadtree() = default;
	Quadtree<T>		*Get(int childIndex);
	bool			Insert(T data, VEC2 min, VEC2 max);
	std::vector<T>	&Data();
	VEC2			Min() {return _min;}
	VEC2			Max() {return _max;}
	int				Level() {return _level;}

private:
	std::array<Quadtree<T> *, 4>	_children {nullptr};
	std::vector<T>	_data;
	VEC2	_min {0, 0};
	VEC2	_max {0, 0};
	VEC2	_mid {0, 0};
	int		_maxLevel {-1};
	int		_level {0};
};

template<typename T>
inline bool				Quadtree<T>::Insert(T data, VEC2 min, VEC2 max)
{
	int		index = -1;
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
	if (min.y > _mid.y) {
		top = true;
		childMin.y = _mid.y;
		childMax.y = _max.y;
	}
	else if (max.y < _mid.y) {
		bottom = true;
		childMin.y = _min.y;
		childMax.y = _mid.y;
	}
	if (max.x < _mid.x) {
		left = true;
		childMin.x = _min.x;
		childMax.x = _mid.x;
	}
	else if (min.x > _mid.x) {
		right = true;
		childMin.x = _mid.x;
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
	if (index == -1 || (_maxLevel > -1 && _level >= _maxLevel)) {
		_data.push_back(data);
		return true;
	}
	_children.at(index) = new Quadtree<T>(childMin, childMax, _maxLevel);
	_children.at(index)->_level = _level + 1;
	return _children.at(index)->Insert(data, min, max);

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