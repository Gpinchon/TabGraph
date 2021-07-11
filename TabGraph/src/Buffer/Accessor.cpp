/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-14 16:24:41
*/
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>

#include <algorithm>

namespace TabGraph::Buffer {
template<typename T>
Accessor<T>::Accessor()
    : Core::Inherit()
{
    size_t s_bufferAccessorNbr = 0;
    SetName("Buffer::Accessor_" + std::to_string(++s_bufferAccessorNbr));
}
}
