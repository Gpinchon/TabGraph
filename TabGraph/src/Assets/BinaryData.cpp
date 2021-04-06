/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-03 17:50:36
*/
#include "Assets/BinaryData.hpp"

BinaryData::BinaryData(const std::vector<std::byte>& data)
    : _data(data)
    , _byteLength(_data.size())
{
}

BinaryData::BinaryData(size_t byteLength)
    : _data(byteLength)
    , _byteLength(_data.size())
{
}

BinaryData::~BinaryData()
{
}

size_t BinaryData::GetByteLength() const
{
    return _byteLength;
}

void BinaryData::SetByteLength(size_t byteLength)
{
    _data.resize(byteLength);
    _byteLength = byteLength;
}
