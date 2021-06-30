/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   Gpinchon
* @Last Modified time: 2020-08-27 17:00:49
*/
#pragma once

#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

#include <filesystem>
#include <memory>
#include <vector>
#include <iterator>
#include <cassert>

namespace TabGraph::Assets {
/** A buffer points to binary geometry, animation, or skins. */
class BinaryData : public Core::Inherit<Core::Object, BinaryData> {
public:
    static constexpr auto AssetType = "BinaryData";
    BinaryData() = delete;
    BinaryData(size_t byteLength)
        : _data(byteLength)
    {
    }
    BinaryData(const std::vector<std::byte>& data)
        : _data(data)
    {
    }
    /** The total byte length of the buffer. */
    inline size_t GetByteLength() const
    {
        return _data.size();
    }
    inline void SetByteLength(size_t byteLength)
    {
        _data.resize(byteLength);
    }

    template<typename T>
    inline void PushBack(const T &data) {
        SetByteLength(GetByteLength() + sizeof(T));
        Set(reinterpret_cast<const std::byte*>(&data), GetByteLength() - sizeof(T), sizeof(T));
    }
    /**
     * @brief returns a reference to the byte casted as T at index * sizeof(T)
     * @tparam T 
     * @param index the index to fetch, will be multiplied by sizeof T
     * @return 
    */
    template<typename T>
    inline T& At(size_t index) {
        return *Get(index * sizeof(T));
    }
    template<typename T>
    inline void Set(const T& data, size_t index) {
        return Set(static_cast<std::byte*>(&data), index, sizeof(T));
    }

    inline void Set(const std::byte* data, size_t index, size_t size) {
        assert(index + size <= _data.size());
        std::memcpy(Get(index), data, size);
    }
    inline std::byte* Get(size_t index) {
        return &_data.at(index);
    }

private:
    std::vector<std::byte> _data { 0 };
};
}
