/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-14 16:24:41
*/
#include "Buffer/Accessor.hpp"
#include "Buffer/View.hpp"
#include <algorithm>

size_t bufferAccessorNbr = 0;

uint8_t GetComponentTypeOctetsSize(const Buffer::Accessor::ComponentType type)
{
    switch (type) {
    case Buffer::Accessor::ComponentType::Int8:
    case Buffer::Accessor::ComponentType::Uint8:
        return 1;
    case Buffer::Accessor::ComponentType::Int16:
    case Buffer::Accessor::ComponentType::Uint16:
        return 2;
    case Buffer::Accessor::ComponentType::Uint32:
    case Buffer::Accessor::ComponentType::Float32:
        return 4;
    default:
        throw std::runtime_error("Unknown Buffer::Accessor::ComponentType");
    }
}

Buffer::Accessor::Buffer::Accessor(const ComponentType componentType, const Type type, const size_t count)
{
    SetType(type);
    SetComponentType(componentType);
    SetCount(count);
    _SetComponentOctetsSize(GetComponentTypeOctetsSize(componentType));
    _SetTypeOctetsSize(GetComponentOctetsSize() * (int)type);
    auto bufferView{ Component::Create<Buffer::View>(count * GetTypeOctetsSize()) };
    bufferView->SetType(Buffer::View::Type::Array);
    SetBufferView(bufferView);
}

Buffer::Accessor::Buffer::Accessor(const ComponentType componentType, const Type type, std::shared_ptr<Buffer::View> bufferView)
{
    SetType(type);
    SetComponentType(componentType);
    _SetComponentOctetsSize(GetComponentTypeOctetsSize(componentType));
    _SetTypeOctetsSize(GetComponentOctetsSize() * (int)type);
    SetCount(bufferView->GetByteLength() / GetTypeOctetsSize());
    SetBufferView(bufferView);
}

std::shared_ptr<Buffer::View> Buffer::Accessor::GetBufferView() const
{
    return _bufferView;
    //return GetComponent<Buffer::View>();
}

void Buffer::Accessor::SetBufferView(std::shared_ptr<Buffer::View> bufferView)
{
    _bufferView = bufferView;
    //SetComponent(bufferView);
}
