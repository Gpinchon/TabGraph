#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
template<typename RegistryType>
class EntityStorage {
public:
    EntityStorage() = default;
private:
    friend RegistryType;
    uint32_t    refCount{ 0 };
};
}