#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Registry.hpp>

#include <set>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class Children {
public:
    template <typename T>
    void insert(const T& a_Entity) { _entities.insert(a_Entity); }
    template <typename T>
    void erase(const T& a_Entity) { _entities.erase(a_Entity); }
    auto begin() { return _entities.begin(); }
    auto end() { return _entities.end(); }

private:
    std::set<ECS::DefaultRegistry::EntityRefType> _entities;
};
}
