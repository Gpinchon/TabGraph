/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 16:09:07
*/

#pragma once

#include "Object.hpp"

#include <algorithm>
#include <vector>

class Component : public Object {
public:
    typedef std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> ComponentMap;
    Component()
        : Object() {};
    Component(const std::string& name)
        : Object(name) {};
    ~Component() = default;
    ComponentMap Components() const;
    /**
     * @brief Searches for the component of the specified type
     * @return true if component is found
     */
    template <typename T>
    bool HasComponentOfType(const std::shared_ptr<T>& component) const;
    /**
     * @brief Searches for the component in all types
     * @return true if component is found
     */
    bool HasComponent(const std::shared_ptr<Component>& component) const;
    /** Attaches the specified component to the object */
    template <typename T>
    void AddComponent(const std::shared_ptr<T>& component);
    /** Sets the first component attached to this object to component, adds it if component type is missing */
    template <typename T>
    void SetComponent(const std::shared_ptr<T>& component);
    template <typename T>
    void SetComponets(const std::vector<std::shared_ptr<T>>& components);
    /** Removes all the components of specified type from the object */
    template <typename T>
    void RemoveComponents();
    /** Removes the specified component from the object */
    template <typename T>
    void RemoveComponent(const std::shared_ptr<T>& component);
    /** @return the first component of the specified type attached to the object, null if not found */
    template <typename T>
    std::shared_ptr<T> GetComponent() const;

    /** @return all components of the specified type with matching id */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsByID(int64_t id) const;
    /** @return all components of the specified type with matching name */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsByName(const std::string& name) const;
    /** @return all components of the specified type with matching id in this component and its children */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsInChildrenByID(int64_t id) const;
    /** @return all components of the specified type with matching id in this component and its children */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsInChildrenByName(const std::string& name) const;

    /** @return the first component of the specified type with matching id */
    template <typename T>
    std::shared_ptr<T> GetComponentByID(int64_t id) const;
    /** @return the first component of the specified type with matching name */
    template <typename T>
    std::shared_ptr<T> GetComponentByName(const std::string& name) const;
    /** @return the first component of the specified type with matching id in this component and its children */
    template <typename T>
    std::shared_ptr<T> GetComponentInChildrenByID(int64_t id) const;
    /** @return the first component of the specified type with matching id in this component and its children */
    template <typename T>
    std::shared_ptr<T> GetComponentInChildrenByName(const std::string& name) const;

    /** @return all components of the specified type attached to the object and it's children */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponentsInChildren() const;
    /** @return all components of the specified type attached to the object */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponents() const;
    /** @returns all components */
    ComponentMap GetComponents() const;
    /** @returns all components in children */
    Component::ComponentMap GetComponentsInChildren() const;

    /** @return types of component attached to this object */
    std::vector<std::type_index> GetComponentsTypes() const;
    virtual bool NeedsFixedUpdateGPU() const final { return _needsFixedUpdateGPU; };
    virtual void SetNeedsFixedUpdateGPU(bool changed) final { _needsFixedUpdateGPU = changed; };
    virtual bool NeedsFixedUpdateCPU() const final { return _needsFixedUpdateCPU; };
    virtual void SetNeedsFixedUpdateCPU(bool changed) final { _needsFixedUpdateCPU = changed; };
    virtual bool NeedsUpdateGPU() const final { return _needsUpdateGPU; };
    virtual void SetNeedsUpdateGPU(bool changed) final { _needsUpdateGPU = changed; };
    virtual bool NeedsUpdateCPU() const final { return _needsUpdateCPU; };
    virtual void SetNeedsUpdateCPU(bool changed) final { _needsUpdateCPU = changed; };
    virtual bool LoadedGPU() const final { return _loadedGPU; };
    virtual void SetLoadedGPU(bool loaded) final { _loadedGPU = loaded; };
    virtual bool LoadedCPU() const final { return _loadedCPU; };
    virtual void SetLoadedCPU(bool loaded) final { _loadedCPU = loaded; };
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
    virtual void UpdateGPU(float delta) final;
    /** Calls FixedUpdateCPU for all sub Components */
    virtual void FixedUpdateCPU(float delta) final;
    /** Calls FixedUpdateGPU for all sub Components */
    virtual void FixedUpdateGPU(float delta) final;

    virtual Component& operator+=(const Component& other)
    {
        for (const auto& otherTypes : other._components) {
            for (const auto& component : otherTypes.second) {
                _components[otherTypes.first].push_back(component);
            }
        }
        return *this;
    }

private:
    virtual void _LoadCPU() = 0;
    virtual void _UnloadCPU() = 0;
    virtual void _LoadGPU() = 0;
    virtual void _UnloadGPU() = 0;
    virtual void _UpdateCPU(float delta) = 0;
    virtual void _UpdateGPU(float delta) = 0;
    virtual void _FixedUpdateCPU(float delta) = 0;
    virtual void _FixedUpdateGPU(float delta) = 0;
    ComponentMap _components;
    bool _needsFixedUpdateGPU { true };
    bool _needsFixedUpdateCPU { true };
    bool _needsUpdateGPU { true };
    bool _needsUpdateCPU { true };
    bool _loadedGPU { false };
    bool _loadedCPU { false };
};

template <typename T>
inline bool Component::HasComponentOfType(const std::shared_ptr<T>& component) const
{
    auto componentsIt = _components.find(typeid(T));
    if (componentsIt != _components.end()) {
        auto& components = componentsIt->second;
        return std::find(components.begin(), components.end(), component) != components.end();
    }
    return false;
}

inline Component::ComponentMap Component::Components() const
{
    return _components;
}

inline bool Component::HasComponent(const std::shared_ptr<Component>& component) const
{
    for (const auto& components : _components) {
        if (std::find(components.second.begin(), components.second.end(), component) != components.second.end())
            return true;
    }
    return false;
}

template <typename T>
inline void Component::AddComponent(const std::shared_ptr<T>& component)
{
    if (component == nullptr)
        return;
    auto& components = _components[typeid(T)];
    auto componentIterator = std::find(components.begin(), components.end(), component);
    if (componentIterator == components.end())
        components.push_back(component);
}

template <typename T>
inline void Component::SetComponent(const std::shared_ptr<T>& component)
{
    if (component == nullptr)
        return;
    auto& components = _components[typeid(T)];
    if (components.empty())
        AddComponent(component);
    else
        components.at(0) = component;
}

template<typename T>
inline void Component::SetComponets(const std::vector<std::shared_ptr<T>>& components)
{
    _components[typeid(T)] = components;
}

template <typename T>
inline void Component::RemoveComponents()
{
    _components.erase(typeid(T));
}

template <typename T>
inline void Component::RemoveComponent(const std::shared_ptr<T>& component)
{
    auto& components = _components[typeid(T)];
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponent() const
{
    auto componentsIt = _components.find(typeid(T));
    if (componentsIt != _components.end()) {
        auto& components = componentsIt->second;
        if (!components.empty())
            return std::static_pointer_cast<T>(components.at(0));
    }
    return nullptr;
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponentsByID(int64_t id) const
{
    std::vector<std::shared_ptr<T>> components;
    auto componentsIt = _components.find(typeid(T));
    if (componentsIt != _components.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Id() == id)
                components.push_back(std::static_pointer_cast<T>(component));
        }
    }
    return components;
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponentsByName(const std::string& name) const
{
    std::vector<std::shared_ptr<T>> components;
    auto componentsIt = _components.find(typeid(T));
    if (componentsIt != _components.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Name() == name)
                components.push_back(std::static_pointer_cast<T>(component));
        }
    }
    return components;
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponentsInChildrenByID(int64_t id) const
{
    std::vector<std::shared_ptr<T>> components;
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Id() == id)
            components.push_back(std::static_pointer_cast<T>(component));
    }
    return components;
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponentsInChildrenByName(const std::string& name) const
{
    std::vector<std::shared_ptr<T>> components;
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Name() == name)
            components.push_back(std::static_pointer_cast<T>(component));
    }
    return components;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentByID(int64_t id) const
{
    auto componentsIt = _components.find(typeid(T));
    if (componentsIt != _components.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Id() == id)
                return std::static_pointer_cast<T>(component);
        }
    }
    return nullptr;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentByName(const std::string& name) const
{
    auto componentsIt = _components.find(typeid(T));
    if (componentsIt != _components.end()) {
        for (const auto& component : componentsIt->second) {
            if (component->Name() == name)
                return std::static_pointer_cast<T>(component);
        }
    }
    return nullptr;
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponentInChildrenByID(int64_t id) const
{
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Id() == id)
            return std::static_pointer_cast<T>(component);
    }
    return nullptr;
}
template <typename T>
inline std::shared_ptr<T> Component::GetComponentInChildrenByName(const std::string& name) const
{
    for (const auto& component : GetComponentsInChildren<T>()) {
        if (component->Name() == name)
            return std::static_pointer_cast<T>(component);
    }
    return nullptr;
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponentsInChildren() const
{
    auto components = GetComponents<T>();
    for (const auto& type : _components) {
        for (const auto& childComponent : type.second) {
            auto childComponents = childComponent->GetComponentsInChildren<T>();
            components.insert(components.begin(), childComponents.begin(), childComponents.end());
        }
    }
    return components;
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponents() const
{
    std::vector<std::shared_ptr<T>> componentsCasted;
    if (_components.find(typeid(T)) != _components.end()) {
        auto& components = _components.at(typeid(T));
        componentsCasted.reserve(components.size());
        for (const auto& component : components)
            componentsCasted.push_back(std::static_pointer_cast<T>(component));
    }
    return componentsCasted;
}

inline Component::ComponentMap Component::GetComponentsInChildren() const
{
    auto components = GetComponents();
    for (const auto& type : _components) {
        for (const auto& childComponent : type.second) {
            auto childComponents = childComponent->GetComponentsInChildren();
            for (auto childType : childComponents) {
                components[childType.first].insert(components[childType.first].begin(), childType.second.begin(), childType.second.end());
            }
        }
    }
    return components;
}

inline Component::ComponentMap Component::GetComponents() const
{
    return _components;
}

inline std::vector<std::type_index> Component::GetComponentsTypes() const
{
    std::vector<std::type_index> types;
    types.reserve(_components.size());
    for (const auto& components : _components) {
        types.push_back(components.first);
    }
    return types;
}

inline void Component::LoadCPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _LoadCPU();
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->LoadCPU();
    }
}

inline void Component::UnloadCPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _UnloadCPU();
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->UnloadCPU();
    }
}

inline void Component::LoadGPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _LoadGPU();
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->LoadGPU();
    }
}

inline void Component::UnloadGPU()
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    _UnloadGPU();
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->UnloadGPU();
    }
}

inline void Component::UpdateCPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (NeedsUpdateCPU())
        _UpdateCPU(delta);
    //SetNeedsUpdateCPU(false);
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->UpdateCPU(delta);
    }
}

inline void Component::UpdateGPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (NeedsUpdateGPU())
        _UpdateGPU(delta);
    //SetNeedsUpdateGPU(false);
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->UpdateGPU(delta);
    }
}

inline void Component::FixedUpdateCPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (NeedsFixedUpdateCPU())
        _FixedUpdateCPU(delta);
    //SetNeedsFixedUpdateCPU(false);
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->FixedUpdateCPU(delta);
    }
}

inline void Component::FixedUpdateGPU(float delta)
{
    //Keep this component alive in case it destroys itself
    auto thisPtr = shared_from_this();
    if (NeedsFixedUpdateGPU())
        _FixedUpdateGPU(delta);
    //SetNeedsFixedUpdateGPU(false);
    for (const auto& components : _components) {
        for (auto& component : components.second)
            component->FixedUpdateGPU(delta);
    }
}