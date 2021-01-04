/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-12-23 17:50:30
*/

#pragma once

#include "Object.hpp"
#include "Tools/Tools.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "Event/Signal.hpp"

/** Use this to declare a new property */
#define PROPERTY(type, var, ...)    \
public:                             \
    Signal<type> var##Changed;      \
    type Get##var() const           \
    {                               \
        return _##var;              \
    }                               \
    void Set##var(const type& val)  \
    {                               \
        if (val != _##var) {        \
            _##var = val;           \
            var##Changed.Emit(val); \
        }                           \
    }                               \
                                    \
private:                            \
    type _##var { __VA_ARGS__ };

#define PRIVATEPROPERTY(type, var, ...) \
private:                                \
    Signal<type> var##Changed;          \
    type Get##var() const               \
    {                                   \
        return _##var;                  \
    }                                   \
    void Set##var(const type& val)      \
    {                                   \
        bool changed = val != _##var;   \
        _##var = val;                   \
        if (changed)                    \
            var##Changed.Emit(val);     \
    }                                   \
    type _##var { __VA_ARGS__ };

class Component : public Object {
    //PROPERTY(bool, NeedsFixedUpdateGPU, true);
    PROPERTY(bool, NeedsFixedUpdateCPU, true);
    PROPERTY(bool, NeedsUpdateGPU, true);
    PROPERTY(bool, NeedsUpdateCPU, true);
    PROPERTY(bool, LoadedGPU, false);
    PROPERTY(bool, LoadedCPU, false);
public:
    std::shared_ptr<Component> Clone();
    typedef std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> ComponentTypesMap;
    Component()
        : Object() {};
    /*Component(const Component& component)
        : Object(component)
        , _components(component._components)
        , _componentsMap(component._componentsMap)
        , _needsFixedUpdateGPU(component._needsFixedUpdateGPU)
        , _needsFixedUpdateCPU(component._needsFixedUpdateCPU)
        , _needsUpdateGPU(component._needsUpdateGPU)
        , _needsUpdateCPU(component._needsUpdateCPU)
        , _loadedGPU(component._loadedGPU)
        , _loadedCPU(component._loadedCPU)
        , _typeIndex(component._typeIndex)
    {
    }*/
    Component(const std::string& name)
        : Object(name) {};
    ~Component() = default;
    /**
     * @brief Searches for the component of the specified type
     * @return true if component is found
     */
    template <typename T, typename = IsSharedPointerOfType<Component, T>>
    bool HasComponentOfType(const std::shared_ptr<T>& component) const;
    template <typename T>
    bool HasComponentOfType() const noexcept
    {
        return _componentsMap.find(typeid(T)) != _componentsMap.end();
    }
    /**
     * @brief Searches for the component in all types
     * @return true if component is found
     */
    bool HasComponent(const std::shared_ptr<Component>& component) const noexcept
    {
        return std::find(GetComponents().begin(), GetComponents().end(), component) != GetComponents().end();
    }
    /** Attaches the specified component to the object */
    template <typename T>
    int64_t AddComponent(const std::shared_ptr<T>& component);
    /** Sets the first component attached to this object to component, adds it if component type is missing */
    template <typename T>
    void SetComponent(const std::shared_ptr<T>& component) noexcept
    {
        SetComponent(typeid(T), component);
    }
    template <typename T>
    void SetComponents(const std::vector<std::shared_ptr<T>>& components) noexcept
    {
        _componentsMap[typeid(T)] = components;
    }
    auto SetComponent(std::type_index type, const std::shared_ptr<Component>& component)
    {
        AddComponent(component);
        auto& comps = _componentsMap[type];
        comps.resize(std::max(comps.size(), size_t(1u)));
        comps.at(0) = component;
    }
    /** Removes all the components of specified type from the object */
    template <typename T>
    void RemoveComponents()
    {
        _componentsMap.erase(typeid(T));
    }
    /** Removes the specified component from the object */
    template <typename T>
    void RemoveComponent(const std::shared_ptr<T>& component);
    /** @return the first component of the specified type attached to the object, null if not found */
    template <typename T>
    std::shared_ptr<T> GetComponent() const
    {
        return std::static_pointer_cast<T>(GetComponent(typeid(T), 0));
    }
    template <typename T>
    std::shared_ptr<T> GetComponent(size_t index) const
    {
        return std::static_pointer_cast<T>(GetComponent(typeid(T), index));
    }
    inline auto GetComponent(std::type_index type, size_t index) const
    {
        if (_componentsMap.count(type) == 0 || index >= _componentsMap.at(type).size())
            return std::shared_ptr<Component>(nullptr);
        return _componentsMap.at(type).at(index);
    }
    inline auto GetComponent(std::type_index type) const
    {
        return GetComponent(type, 0);
    }

    /** @return all components of the specified type with matching id */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsByID(int64_t id) const noexcept;
    /** @return all components of the specified type with matching name */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsByName(const std::string& name) const noexcept;
    /** @return all components of the specified type with matching id in this component and its children */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsInChildrenByID(int64_t id) const noexcept;
    /** @return all components of the specified type with matching id in this component and its children */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsInChildrenByName(const std::string& name) const noexcept;

    /** @return the first component of the specified type with matching id */
    template <typename T>
    std::shared_ptr<T> GetComponentByID(int64_t id) const noexcept;
    /** @return the first component of the specified type with matching name */
    template <typename T>
    std::shared_ptr<T> GetComponentByName(const std::string& name) const noexcept;
    /** @return the first component of the specified type with matching id in this component and its children */
    template <typename T>
    std::shared_ptr<T> GetComponentInChildrenByID(int64_t id) const noexcept;
    /** @return the first component of the specified type with matching id in this component and its children */
    template <typename T>
    std::shared_ptr<T> GetComponentInChildrenByName(const std::string& name) const noexcept;

    template <typename T>
    int64_t GetComponentIndex(const std::shared_ptr<T>& component) const noexcept
    {
        return GetComponentIndex(typeid(T), component);
    }

    /** @return the number of components of the specified type */
    template <typename T = Component>
    size_t GetComponentsNbr() const noexcept;
    /** @return all components of the specified type attached to the object */
    template <typename T = Component>
    std::vector<std::shared_ptr<T>> GetComponents() const noexcept;
    /** @return all components of the specified type attached to the object and it's children */
    template <typename T = Component>
    std::unordered_set<std::shared_ptr<T>> GetComponentsInChildren() const noexcept;

    /** @return the number of components */
    auto GetComponentsNbr() const noexcept
    {
        return GetComponents().size();
    }
    /** @returns all components */
    const std::vector<std::shared_ptr<Component>>& GetComponents() const noexcept
    {
        return _components;
    }
    std::vector<std::shared_ptr<Component>>& GetComponents() noexcept
    {
        return _components;
    }

    /** @returns all components in children */
    std::unordered_set<std::shared_ptr<Component>> GetComponentsInChildren() const noexcept;

    /** @return types of component attached to this object */
    std::unordered_set<std::type_index> GetComponentsTypes() const noexcept;

    /** Calls LoadCPU for all sub Components */
    virtual void LoadCPU() final;
    /** Calls UnloadCPU for all sub Components */
    virtual void UnloadCPU() final;
    /** Calls LoadGPU for all sub Components */
    virtual void LoadGPU() final;
    /** Calls UnloadGPU for all sub Components */
    virtual void UnloadGPU() final;
    /** Calls UpdateCPU for all sub Components */
    virtual void UpdateCPU(float delta) final;
    /** Calls UpdateGPU for all sub Components */
    //virtual void UpdateGPU(float delta) final;
    /** Calls FixedUpdateCPU for all sub Components */
    virtual void FixedUpdateCPU(float delta) final;
    /** Calls FixedUpdateGPU for all sub Components */
    //virtual void FixedUpdateGPU(float delta) final;

    virtual std::shared_ptr<Component> operator+=(const std::shared_ptr<Component>& other)
    {
        if (other != nullptr) {
            for (const auto& otherTypes : other->_componentsMap) {
                for (const auto& component : otherTypes.second) {
                    AddComponent(otherTypes.first, component);
                }
            }
        }
        return std::static_pointer_cast<Component>(shared_from_this());
    }

    virtual std::shared_ptr<Component> operator+(const std::shared_ptr<Component>& other)
    {
        auto clone = _Clone();
        *clone += other;
        return clone;
    }

    template <class T, typename... Params>
    static std::shared_ptr<T> Create(Params&&... args)
    {
        auto component = std::make_shared<T>(args...);
        component->_typeIndex = typeid(T);
        return component;
        //component->GetComponentManager()->AddComponent(component);
        //return Reference(component, ComponentManager::CurrentManager());
    }

    void Replace(const std::shared_ptr<Component>& oldComponent, const std::shared_ptr<Component>& newComponent)
    {
        _Replace(oldComponent, newComponent);
        for (auto& type : _componentsMap) {
            std::replace(type.second.begin(), type.second.end(), oldComponent, newComponent);
        }
        std::replace(GetComponents().begin(), GetComponents().end(), oldComponent, newComponent);
    }

    template <typename T>
    void Replace(const std::shared_ptr<T>& oldComponent, const std::shared_ptr<T>& newComponent)
    {
        _Replace(oldComponent, newComponent);
        auto componentsIt = _componentsMap.find(typeid(T));
        if (componentsIt != _componentsMap.end())
            std::replace(componentsIt->second.begin(), componentsIt->second.end(), oldComponent, newComponent);
        std::replace(GetComponents().begin(), GetComponents().end(), oldComponent, newComponent);
    }

private:
    void AddToGetComponents(const std::shared_ptr<Component>& component)
    {
        if (std::find(GetComponents().begin(), GetComponents().end(), component) == GetComponents().end())
            GetComponents().emplace_back(component);
    }
    int64_t GetComponentIndex(std::type_index typeIndex, const std::shared_ptr<Component>& component) const noexcept
    {
        auto componentsIt = _componentsMap.find(typeIndex);
        if (componentsIt == _componentsMap.end())
            return -1;
        auto componentIt = std::find(componentsIt->second.begin(), componentsIt->second.end(), component);
        if (componentIt == componentsIt->second.end())
            return -1;
        return std::distance(componentsIt->second.begin(), componentIt);
    }
    int64_t AddComponent(std::type_index typeIndex, const std::shared_ptr<Component>& component) noexcept
    {
        AddToGetComponents(component);
        if (std::find(_componentsMap[typeIndex].begin(), _componentsMap[typeIndex].end(), component) == _componentsMap[typeIndex].end()) {
            _componentsMap[typeIndex].emplace_back(component);
        }
        return GetComponentIndex(typeIndex, component);
    }
    virtual void _Replace(const std::shared_ptr<Component>& oldComponent, const std::shared_ptr<Component>& newComponent) {};
    virtual std::shared_ptr<Component> _Clone() = 0;
    virtual void _LoadCPU() = 0;
    virtual void _UnloadCPU() = 0;
    virtual void _LoadGPU() = 0;
    virtual void _UnloadGPU() = 0;
    virtual void _UpdateCPU(float delta) = 0;
    //virtual void _UpdateGPU(float delta) = 0;
    virtual void _FixedUpdateCPU(float delta) = 0;
    //virtual void _FixedUpdateGPU(float delta) = 0;
    //std::weak_ptr<Component> _parent;
    std::vector<std::shared_ptr<Component>> _components;
    ComponentTypesMap _componentsMap;
    std::type_index _typeIndex { typeid(*this) };
};

template <typename T, typename = IsSharedPointerOfType<Component, T>>
inline auto operator+=(std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
{
    return std::static_pointer_cast<T>(a->operator+=(b));
}

template <typename T, typename = IsSharedPointerOfType<Component, T>>
inline auto operator+(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
{
    return std::static_pointer_cast<T>(a->operator+(b));
}

template <typename T, typename>
inline bool Component::HasComponentOfType(const std::shared_ptr<T>& component) const
{
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        auto& components = componentsIt->second;
        return std::find(components.begin(), components.end(), component) != components.end();
    }
    return false;
}

#include "Debug.hpp"

inline std::shared_ptr<Component> Component::Clone()
{
    auto clone = _Clone();
    debugLog(typeid(*this).name());
    debugLog(typeid(*clone).name());
    if (_typeIndex != clone->_typeIndex)
        throw std::runtime_error(_typeIndex.name() + std::string(" != ") + clone->_typeIndex.name());
    //clone->Replace(std::static_pointer_cast<Component>(shared_from_this()), clone);
    auto components = GetComponentsInChildren();
    std::vector<std::pair<std::shared_ptr<Component>, std::shared_ptr<Component>>> newComponents;
    newComponents.reserve(components.size());
    for (const auto& oldComponent : components) {
        auto newComponent = oldComponent->_Clone();
        if (oldComponent->_typeIndex != newComponent->_typeIndex)
            throw std::runtime_error(oldComponent->_typeIndex.name() + std::string(" != ") + newComponent->_typeIndex.name());
        newComponent->Replace(std::static_pointer_cast<Component>(shared_from_this()), clone);
        clone->Replace(oldComponent, newComponent);
        newComponents.emplace_back(oldComponent, newComponent);
    }
    for (auto componentA : newComponents) {
        for (auto componentB : newComponents) {
            componentB.second->Replace(componentA.first, componentA.second);
        }
    }
    return clone;
}

template <typename T>
inline int64_t Component::AddComponent(const std::shared_ptr<T>& component)
{
    if (component == nullptr || component.get() == this)
        return -1;
    return AddComponent(typeid(T), component);
}
/*
template <typename T>
inline void Component::SetComponent(const std::shared_ptr<T>& component) noexcept
{
    if (component == nullptr || component.get() == this)
        return;
    if (_componentsMap[typeid(T)].empty())
        AddComponent(typeid(T), component);
    else {
        AddToGetComponents(component);
        _componentsMap[typeid(T)].at(0) = component;
    }
}
*/
template <typename T>
inline void Component::RemoveComponent(const std::shared_ptr<T>& component)
{
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt == _componentsMap.end())
        return;
    const auto last = std::remove(componentsIt->second.begin(), componentsIt->second.end(), component);
    if (last == componentsIt->second.end())
        return;
    componentsIt->second.erase(last, componentsIt->second.end());
    if (component.use_count() == 2) //There only is the ptr inside vector and "component" variable
        GetComponents().erase(std::remove(GetComponents().begin(), GetComponents().end(), component), GetComponents().end());
}
/*
template <typename T>
inline std::shared_ptr<T> Component::GetComponent() const noexcept
{
    std::shared_ptr<T> ptr;
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        auto& components = componentsIt->second;
        if (!components.empty())
            ptr = std::static_pointer_cast<T>(components.at(0));
    }
    return ptr;
}

template<typename T>
inline std::shared_ptr<T> Component::GetComponent(size_t index) const noexcept
{
    std::shared_ptr<T> ptr;
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        auto& components = componentsIt->second;
        if (!components.empty() && components.size() >= index)
            ptr = std::static_pointer_cast<T>(components.at(index));
    }
    return ptr;
}
*/
template <typename T>
inline std::unordered_set<std::shared_ptr<T>> Component::GetComponentsByID(int64_t id) const noexcept
{
    std::unordered_set<std::shared_ptr<T>> components;
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Id() == id)
                components.insert(std::static_pointer_cast<T>(component));
        }
    }
    return components;
}

template <typename T>
inline std::unordered_set<std::shared_ptr<T>> Component::GetComponentsByName(const std::string& name) const noexcept
{
    std::unordered_set<std::shared_ptr<T>> components;
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Name() == name)
                components.insert(std::static_pointer_cast<T>(component));
        }
    }
    return components;
}

template <typename T>
inline std::unordered_set<std::shared_ptr<T>> Component::GetComponentsInChildrenByID(int64_t id) const noexcept
{
    std::unordered_set<std::shared_ptr<T>> components;
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Id() == id)
            components.insert(std::static_pointer_cast<T>(component));
    }
    return components;
}

template <typename T>
inline std::unordered_set<std::shared_ptr<T>> Component::GetComponentsInChildrenByName(const std::string& name) const noexcept
{
    std::unordered_set<std::shared_ptr<T>> components;
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Name() == name)
            components.push_back(std::static_pointer_cast<T>(component));
    }
    return components;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentByID(int64_t id) const noexcept
{
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Id() == id)
                return std::static_pointer_cast<T>(component);
        }
    }
    return nullptr;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentByName(const std::string& name) const noexcept
{
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Name() == name)
                return std::static_pointer_cast<T>(component);
        }
    }
    return nullptr;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentInChildrenByID(int64_t id) const noexcept
{
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Id() == id)
            return std::static_pointer_cast<T>(component);
    }
    return nullptr;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentInChildrenByName(const std::string& name) const noexcept
{
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Name() == name)
            return std::static_pointer_cast<T>(component);
    }
    return nullptr;
}

template <typename T>
inline size_t Component::GetComponentsNbr() const noexcept
{
    auto componentsIt = _componentsMap.find(typeid(T));
    return (componentsIt == _componentsMap.end()) ? 0 : componentsIt->second.size();
}

#include <algorithm>
#include <iterator>

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponents() const noexcept
{
    std::vector<std::shared_ptr<T>> vec;
    auto it = _componentsMap.find(typeid(T));
    if (it != _componentsMap.end()) {
        vec.reserve(it->second.size());
        std::transform(it->second.begin(), it->second.end(), std::back_inserter(vec),
            [](const std::shared_ptr<Component>& shptr) {
                return std::static_pointer_cast<T>(shptr);
            });
        //return std::unordered_set<std::shared_ptr<T>>(it->second.begin(), it->second.end());
    }
    return vec;
}

template <typename T>
inline std::unordered_set<std::shared_ptr<T>> Component::GetComponentsInChildren() const noexcept
{
    auto components = GetComponents<T>();
    for (const auto& child : GetComponents()) {
        auto childComponents = child->GetComponentsInChildren<T>();
        components.insert(components.end(), childComponents.begin(), childComponents.end());
    }
    return std::unordered_set<std::shared_ptr<T>>(components.begin(), components.end());
}

inline std::unordered_set<std::shared_ptr<Component>> Component::GetComponentsInChildren() const noexcept
{
    auto components = GetComponents();
    for (const auto& component : GetComponents()) {
        auto childComponents = component->GetComponentsInChildren();
        components.insert(components.end(), childComponents.begin(), childComponents.end());
    }
    return std::unordered_set<std::shared_ptr<Component>>(components.begin(), components.end());
}

inline std::unordered_set<std::type_index> Component::GetComponentsTypes() const noexcept
{
    std::unordered_set<std::type_index> types;
    types.reserve(_componentsMap.size());
    for (const auto& components : _componentsMap) {
        types.insert(components.first);
    }
    return types;
}

inline void Component::LoadCPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _LoadCPU();
    for (const auto& component : GetComponents()) {
        component->LoadCPU();
    }
}

inline void Component::UnloadCPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _UnloadCPU();
    for (const auto& component : GetComponents()) {
        component->UnloadCPU();
    }
}

inline void Component::LoadGPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _LoadGPU();
    for (const auto& component : GetComponents()) {
        component->LoadGPU();
    }
}

inline void Component::UnloadGPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _UnloadGPU();
    for (const auto& component : GetComponents()) {
        component->UnloadGPU();
    }
}

inline void Component::UpdateCPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (GetNeedsUpdateCPU())
        _UpdateCPU(delta);
    //SetNeedsUpdateCPU(false);
    for (const auto& component : GetComponents()) {
        component->UpdateCPU(delta);
    }
}
/*
inline void Component::UpdateGPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (GetNeedsUpdateGPU())
        _UpdateGPU(delta);
    //SetNeedsUpdateGPU(false);
    for (const auto& component : GetComponents()) {
        component->UpdateGPU(delta);
    }
}
*/
inline void Component::FixedUpdateCPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (GetNeedsFixedUpdateCPU())
        _FixedUpdateCPU(delta);
    //SetNeedsFixedUpdateCPU(false);
    for (const auto& component : GetComponents()) {
        component->FixedUpdateCPU(delta);
    }
}
/*
inline void Component::FixedUpdateGPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (GetNeedsFixedUpdateGPU())
        _FixedUpdateGPU(delta);
    //SetNeedsFixedUpdateGPU(false);
    for (const auto& component : GetComponents()) {
        component->FixedUpdateGPU(delta);
    }
}
*/