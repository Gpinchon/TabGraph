/*
* @Author: gpi
* @Date:   2019-06-07 13:35:27
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-07 14:22:21
*/

#pragma once

#include <array>
#include <vml.h>

template<typename T>
class Quadtree
{
public:
	Quadtree(T data, VEC2 min, VEC2 max) : _data(data), _min(min), _max(max) {};
	Quadtree() = delete;
	~Quadtree();
	Quadtree<T>	*Get(int childIndex);
	bool		Insert(Quadtree<T> *);
	T			&Data();

private:
	std::array<Quadtree<T> *, 4>	_children;
	T		_data;
	VEC2	_min {0, 0};
	VEC2	_max {0, 0};
	VEC2	_size{0, 0};
};

template<typename T>
inline bool Quadtree<T>::Insert(Quadtree<T> *tree)
{
	if (tree == nullptr)
		return false;
	if (tree->min.x < (max.x - size.x / 2.f))
	{
		if (tree->min.y < (max.y - size.y / 2.f)) {
			_children.at(0) = tree;
		}
		else {
			_children.at(1) = tree;
		}
	}
	else
	{
		if (tree->min.y > (max.y - size.y / 2.f)) {
			_children.at(2) = tree;
		}
		else {
			_children.at(3) = tree;
		}
	}
	return true;
}

template<typename T>
inline Quadtree<T>	*Quadtree<T>::Get(int childIndex)
{
	return _children.at(childIndex);
}

template<typename T>
inline T			&Quadtree<T>::Data()
{
	return _data;
}