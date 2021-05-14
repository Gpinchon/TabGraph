/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-14 16:24:41
*/
#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferView.hpp"
#include <algorithm>

size_t bufferAccessorNbr = 0;

uint8_t GetComponentTypeOctetsSize(const BufferAccessor::ComponentType type)
{
    switch (type) {
    case BufferAccessor::ComponentType::Int8:
    case BufferAccessor::ComponentType::Uint8:
        return 1;
    case BufferAccessor::ComponentType::Int16:
    case BufferAccessor::ComponentType::Uint16:
        return 2;
    case BufferAccessor::ComponentType::Uint32:
    case BufferAccessor::ComponentType::Float32:
        return 4;
    default:
        throw std::runtime_error("Unknown BufferAccessor::ComponentType");
    }
}

BufferAccessor::BufferAccessor(const ComponentType componentType, const Type type, const size_t count)
{
    SetType(type);
    SetComponentType(componentType);
    SetCount(count);
    _SetComponentOctetsSize(GetComponentTypeOctetsSize(componentType));
    _SetTypeOctetsSize(GetComponentOctetsSize() * (int)type);
    auto bufferView{ Component::Create<BufferView>(count * GetTypeOctetsSize()) };
    bufferView->SetType(BufferView::Type::Array);
    SetBufferView(bufferView);
}

BufferAccessor::BufferAccessor(const ComponentType componentType, const Type type, std::shared_ptr<BufferView> bufferView)
{
    SetType(type);
    SetComponentType(componentType);
    _SetComponentOctetsSize(GetComponentTypeOctetsSize(componentType));
    _SetTypeOctetsSize(GetComponentOctetsSize() * (int)type);
    SetCount(bufferView->GetByteLength() / GetTypeOctetsSize());
    SetBufferView(bufferView);
}

std::shared_ptr<BufferView> BufferAccessor::GetBufferView() const
{
    return _bufferView;
    //return GetComponent<BufferView>();
}

void BufferAccessor::SetBufferView(std::shared_ptr<BufferView> bufferView)
{
    _bufferView = bufferView;
    //SetComponent(bufferView);
}
