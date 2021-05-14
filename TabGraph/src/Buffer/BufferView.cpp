/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 17:47:00
*/
#include "Buffer/BufferView.hpp"
#include "Assets/Asset.hpp"
#include "Assets/BinaryData.hpp"
#include "Renderer/Renderer.hpp"

//#ifdef OPENGL
#include "Driver/OpenGL/Buffer.hpp"
//#endif

size_t s_bufferViewNbr = 0;

BufferView::BufferView(size_t byteLength, std::shared_ptr<Asset> buffer, Mode mode)
    : BufferView(byteLength, mode)
{
    SetComponent(buffer);
}

BufferView::BufferView() : Component("BufferView_" + std::to_string(++s_bufferViewNbr))
{
    SetStorage(GetStorage());
}

BufferView::BufferView(std::shared_ptr<Asset> buffer, Mode mode)
    : BufferView()
{
    SetComponent(buffer);
    SetMode(mode);
}

BufferView::BufferView(size_t byteLength, Mode mode) : BufferView()
{
    SetByteLength(byteLength);
    SetMode(mode);
}

std::byte* BufferView::Get(size_t index, size_t size)
{
    if (GetStorage() == Storage::CPU)
        return _rawData.data() + index;
    else
        return GetImplGPU()->Get(*this, index, size);
}

void BufferView::Set(std::byte* data, size_t index, size_t size)
{
    if (GetStorage() == Storage::CPU)
        std::copy(data, data + size, _rawData.data() + index);
    else
        GetImplGPU()->Set(*this, data, index, size);
}

std::byte* BufferView::MapRange(MappingMode mappingMode, size_t start, size_t end, bool invalidate)
{
    assert(mappingMode != MappingMode::None);
    assert(GetStorage() != Storage::CPU);
    return GetImplGPU()->MapRange(*this, mappingMode, start, end, invalidate);
}

void BufferView::Unmap()
{
    assert(GetStorage() != Storage::CPU);
    GetImplGPU()->Unmap(*this);
}

void BufferView::FlushRange(size_t start, size_t end)
{
    assert(GetType() != Type::Unknown);
    assert(GetStorage() != Storage::CPU);
    GetImplGPU()->FlushRange(*this, start, end);
}

void BufferView::Bind()
{
    assert(GetType() != Type::Unknown);
    assert(GetStorage() != Storage::CPU);
    GetImplGPU()->Bind(*this);
}

size_t BufferView::GetMappingEnd()
{
    assert(GetType() != Type::Unknown);
    assert(GetStorage() != Storage::CPU);
    return GetImplGPU()->GetMappingEnd();
}

size_t BufferView::GetMappingStart()
{
    assert(GetType() != Type::Unknown);
    assert(GetStorage() != Storage::CPU);
    return GetImplGPU()->GetMappingStart();
}

void BufferView::BindNone(BufferView::Type type)
{
    assert(type != Type::Unknown);
    BufferView::ImplGPU::BindNone(type);
}

void BufferView::Done()
{
    BindNone(GetType());
}

void BufferView::Load()
{
    if (GetLoaded())
        return;
    std::byte* bufferData{ nullptr };
    auto bufferAsset{ GetComponent<Asset>() };
    if (bufferAsset != nullptr) {
        bufferAsset->Load();
        auto bufferAssetData = bufferAsset->GetComponent<BinaryData>();
        assert(bufferAssetData != nullptr);
        if (GetByteLength() == 0) //We do not know this BufferView's length yet
            SetByteLength(bufferAssetData->GetByteLength());
        bufferData = bufferAssetData->Get(GetByteOffset());
    }
    if (GetStorage() == Storage::CPU) {
        if (bufferData != nullptr)
            _rawData = std::vector<std::byte>(bufferData, bufferData + GetByteLength());
        else
            _rawData = std::vector<std::byte>(GetByteLength());
    }
    else
        GetImplGPU()->Load(*this, bufferData);
    if (bufferAsset != nullptr)
        RemoveComponent<Asset>(bufferAsset);
    _SetLoaded(true);
}

void BufferView::Unload()
{
    if (GetStorage() == Storage::CPU) {
        _rawData.clear();
        _rawData.shrink_to_fit();
    }
    else
        GetImplGPU()->Unload();
    _SetLoaded(false);
}

void BufferView::SetStorage(Storage storage)
{
    if (storage == Storage::GPU) {
        if (GetImplGPU() == nullptr)
            _SetImplGPU(std::make_shared<BufferView::ImplGPU>());
        if (GetLoaded()) {
            GetImplGPU()->Load(*this, _rawData.data());
            _rawData.clear();
            _rawData.shrink_to_fit();
        }
    }
    else if (storage == Storage::CPU) {
        //repatriate data from GPU if possible
        if (GetImplGPU() != nullptr) {
            if (GetLoaded() && GetMode() != Mode::Immutable) {
                auto data{ GetImplGPU()->MapRange(*this, MappingMode::ReadOnly, 0, GetByteLength()) };
                _rawData = { data, data + GetByteLength() };
            }
            _SetImplGPU(nullptr);
        }
    }
    _SetStorage(storage);
}
