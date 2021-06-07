/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   Gpinchon
* @Last Modified time: 2020-08-27 17:00:49
*/
#pragma once

#include "Component.hpp"

#include <filesystem>
#include <memory>
#include <vector>
#include <iterator>

/** A buffer points to binary geometry, animation, or skins. */
class BinaryData : public Component {
public:
    static constexpr auto AssetType = "BinaryData";
    BinaryData() = delete;
    BinaryData(size_t byteLength);
    BinaryData(const std::vector<std::byte>&);
    ~BinaryData();
    /** The total byte length of the buffer. */
    size_t GetByteLength() const;
    /** Sets the buffer's byte length and RESIZE RAW DATA !!! */
    void SetByteLength(size_t);

    template<typename T>
    void PushBack(const T &data) {
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
    T& At(size_t index) {
        return *Get(index * sizeof(T));
    }

    void Set(const std::byte* data, size_t index, size_t size) {
        assert(index + size <= _data.size());
        std::memcpy(Get(index), data, size);
    }
    std::byte* Get(size_t index) {
        assert(index < _data.size());
        return _data.data() + index;
    }

private:
    std::vector<std::byte> _data { 0 };
    size_t _byteLength{ 0 };
    // Hérité via Component
    virtual std::shared_ptr<Component> _Clone() override
    {
        return std::static_pointer_cast<Component>(shared_from_this());
    }
};
