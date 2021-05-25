/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 14:26:36
*/

#pragma once

#include <Debug.hpp>
#include <Object.hpp>
#include <Tools/Tools.hpp>

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <typeindex>

class Component : public Object {
    //PROPERTY(bool, NeedsFixedUpdateGPU, true);
    /*PROPERTY(bool, NeedsFixedUpdateCPU, true);
    PROPERTY(bool, NeedsUpdateGPU, true);
    PROPERTY(bool, NeedsUpdateCPU, true);
    PROPERTY(bool, LoadedGPU, false);
    PROPERTY(bool, LoadedCPU, false);*/

public:
    std::shared_ptr<Component> Clone();
    typedef std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> ComponentTypesMap;
    Component()
        : Object() {};
    Component(const std::string& name)
        : Object(name) {};
    ~Component() = default;
    /**
     * @brief Searches for the component of the specified type
     * @tparam T : the type of component to search for
     * @param component the component to search for
     * @return true if component is found
    */
    template <typename T, typename = IsSharedPointerOfType<Component, T>>
    bool HasComponentOfType(std::shared_ptr<T> component) const;
    /**
     * @brief Searches for a specific type
     * @tparam T the type of component to search for
     * @return true if the type was found
    */
    template <typename T>
    bool HasComponentOfType() const noexcept;
    /**
     * @brief Searches for the component in all types
     * @param component 
     * @return true if component is found
    */
    bool HasComponent(std::shared_ptr<Component> component) const noexcept;
    /**
     * @brief Attaches the specified component to the object
     * @tparam T 
     * @param component 
     * @return 
    */
    template <typename T>
    int64_t AddComponent(std::shared_ptr<T> component);
    /**
     * @brief Sets the first component attached to this object to component, adds it if component type is missing
     * @tparam T 
     * @param component 
     * @return 
    */
    template <typename T>
    void SetComponent(std::shared_ptr<T> component) noexcept;
    /**
     * @brief TODO
     * @tparam T 
     * @param components 
     * @return 
    */
    template <typename T>
    void SetComponents(const std::vector<std::shared_ptr<T>>& components) noexcept;
    /**
     * @brief TODO
     * @param type 
     * @param component 
     * @return 
    */
    auto SetComponent(std::type_index type, std::shared_ptr<Component> component);
    /** Removes all the components of specified type from the object */
    template <typename T>
    void RemoveComponents();
    /** Removes the specified component from the object */
    template <typename T>
    void RemoveComponent(std::shared_ptr<T> component);
    /**
     * @brief 
     * @tparam T 
     * @return the first component of the specified type attached to the object, null if not found
    */
    template <typename T>
    std::shared_ptr<T> GetComponent() const;
    /**
     * @brief TODO
     * @tparam T 
     * @param index 
     * @return 
    */
    template <typename T>
    std::shared_ptr<T> GetComponent(size_t index) const;
    /**
     * @brief TODO
     * @param type 
     * @param index 
     * @return 
    */
    inline auto GetComponent(std::type_index type, size_t index) const;
    /**
     * @brief TODO
     * @param type 
     * @return 
    */
    inline auto GetComponent(std::type_index type) const;
    /**
     * @brief 
     * @tparam T 
     * @param id 
     * @return all components of the specified type with matching id
    */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsByID(int64_t id) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param name 
     * @return all components of the specified type with matching name
    */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsByName(const std::string& name) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param id 
     * @return all components of the specified type with matching id in this component and its children
    */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsInChildrenByID(int64_t id) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param name 
     * @return all components of the specified type with matching name in this component and its children
    */
    template <typename T>
    std::unordered_set<std::shared_ptr<T>> GetComponentsInChildrenByName(const std::string& name) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param id 
     * @return the first component of the specified type with matching id
    */
    template <typename T>
    std::shared_ptr<T> GetComponentByID(int64_t id) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param name 
     * @return the first component of the specified type with matching name
    */
    template <typename T>
    std::shared_ptr<T> GetComponentByName(const std::string& name) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param id 
     * @return the first component of the specified type with matching id in this component and its children
    */
    template <typename T>
    std::shared_ptr<T> GetComponentInChildrenByID(int64_t id) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @param name 
     * @return the first component of the specified type with matching id in this component and its children
    */
    template <typename T>
    std::shared_ptr<T> GetComponentInChildrenByName(const std::string& name) const noexcept;
    /**
     * @brief TODO
     * @tparam T 
     * @param component 
     * @return 
    */
    template <typename T>
    int64_t GetComponentIndex(std::shared_ptr<T> component) const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @return the number of components of the specified type
    */
    template <typename T = Component>
    size_t GetComponentsNbr() const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @return all components of the specified type attached to the object
    */
    template <typename T = Component>
    std::vector<std::shared_ptr<T>> GetComponents() const noexcept;
    /**
     * @brief 
     * @tparam T 
     * @return all components of the specified type attached to the object and it's children
    */
    template <typename T = Component>
    std::unordered_set<std::shared_ptr<T>> GetComponentsInChildren() const noexcept;
    /**
     * @brief 
     * @return the number of components attached to this Component
    */
    auto GetComponentsNbr() const noexcept;
    /**
     * @brief 
     * @return all components attached to this Component
    */
    std::unordered_set<std::shared_ptr<Component>> GetComponents() const noexcept;
    /**
     * @brief non const overload
     * @return all components attached to this Component
    */
    std::unordered_set<std::shared_ptr<Component>> GetComponents() noexcept;
    /**
     * @brief 
     * @return all components attached to this Component and its children
    */
    std::unordered_set<std::shared_ptr<Component>> GetComponentsInChildren() const noexcept;
    /**
     * @brief 
     * @return types of Component attached to this object
    */
    std::unordered_set<std::type_index> GetComponentsTypes() const noexcept;
    /**
     * @brief Creates a shared_ptr of the specified Component type 
     * @tparam T a type inheriting Component
     * @tparam ...Params
     * @param ...args the parameters to be used for the construction
     * @return a shared_ptr pointing to the Component created
    */
    template <class T, typename... Params>
    static std::shared_ptr<T> Create(Params&&... args);
    /**
     * @brief TODO
     * @param oldComponent 
     * @param newComponent 
    */
    void Replace(std::shared_ptr<Component> oldComponent, std::shared_ptr<Component> newComponent);
    /**
     * @brief TODO
     * @tparam T 
     * @param oldComponent 
     * @param newComponent 
    */
    template <typename T>
    void Replace(std::shared_ptr<T> oldComponent, std::shared_ptr<T> newComponent);

    virtual std::shared_ptr<Component> operator+=(std::shared_ptr<Component> other)
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

    virtual std::shared_ptr<Component> operator+(std::shared_ptr<Component> other)
    {
        auto clone = _Clone();
        *clone += other;
        return clone;
    }

private:
    int64_t GetComponentIndex(std::type_index typeIndex, std::shared_ptr<Component> component) const noexcept;
    int64_t AddComponent(std::type_index typeIndex, std::shared_ptr<Component> component) noexcept;
    virtual void _Replace(std::shared_ptr<Component> oldComponent, std::shared_ptr<Component> newComponent) {};
    virtual std::shared_ptr<Component> _Clone() = 0;
    ComponentTypesMap _componentsMap;
    std::type_index _typeIndex { typeid(*this) };
};

template <typename T, typename = IsSharedPointerOfType<Component, T>>
inline auto operator+=(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
    return std::static_pointer_cast<T>(a->operator+=(b));
}

template <typename T, typename = IsSharedPointerOfType<Component, T>>
inline auto operator+(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
    return std::static_pointer_cast<T>(a->operator+(b));
}

template <typename T, typename>
inline bool Component::HasComponentOfType(std::shared_ptr<T> component) const
{
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end()) {
        auto& components = componentsIt->second;
        return std::find(components.begin(), components.end(), component) != components.end();
    }
    return false;
}

template <typename T>
inline bool Component::HasComponentOfType() const noexcept
{
    return _componentsMap.find(typeid(T)) != _componentsMap.end();
}

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

inline bool Component::HasComponent(std::shared_ptr<Component> component) const noexcept
{
    auto components { GetComponents() };
    return std::find(components.begin(), components.end(), component) != components.end();
}

inline auto Component::SetComponent(std::type_index type, std::shared_ptr<Component> component)
{
    //AddComponent(component);
    auto& comps = _componentsMap[type];
    comps.resize(std::max(comps.size(), size_t(1u)));
    comps.at(0) = component;
}

inline auto Component::GetComponent(std::type_index type, size_t index) const
{
    if (_componentsMap.count(type) == 0 || index >= _componentsMap.at(type).size())
        return std::shared_ptr<Component>(nullptr);
    return _componentsMap.at(type).at(index);
}

inline auto Component::GetComponent(std::type_index type) const
{
    return GetComponent(type, 0);
}

template <typename T>
inline int64_t Component::AddComponent(std::shared_ptr<T> component)
{
    if (component == nullptr || component.get() == this)
        return -1;
    return AddComponent(typeid(T), component);
}

template <typename T>
inline void Component::SetComponent(std::shared_ptr<T> component) noexcept
{
    SetComponent(typeid(T), component);
}

template <typename T>
inline void Component::SetComponents(const std::vector<std::shared_ptr<T>>& components) noexcept
{
    _componentsMap[typeid(T)] = components;
}

template <typename T>
inline void Component::RemoveComponents()
{
    _componentsMap.erase(typeid(T));
}

template <typename T>
inline void Component::RemoveComponent(std::shared_ptr<T> component)
{
    auto& componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt == _componentsMap.end())
        return;
    const auto& last = std::remove(componentsIt->second.begin(), componentsIt->second.end(), component);
    if (last == componentsIt->second.end())
        return;
    componentsIt->second.erase(last, componentsIt->second.end());
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponent() const
{
    return std::static_pointer_cast<T>(GetComponent(typeid(T), 0));
}

template <typename T>
inline std::shared_ptr<T> Component::GetComponent(size_t index) const
{
    return std::static_pointer_cast<T>(GetComponent(typeid(T), index));
}

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
            if (component->GetId() == id)
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
            if (component->GetName() == name)
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
        if (component->GetName() == name)
            return std::static_pointer_cast<T>(component);
    }
    return nullptr;
}

template <typename T>
inline int64_t Component::GetComponentIndex(std::shared_ptr<T> component) const noexcept
{
    return GetComponentIndex(typeid(T), component);
}

template <typename T>
inline size_t Component::GetComponentsNbr() const noexcept
{
    auto componentsIt = _componentsMap.find(typeid(T));
    return (componentsIt == _componentsMap.end()) ? 0 : componentsIt->second.size();
}

template <typename T>
inline std::vector<std::shared_ptr<T>> Component::GetComponents() const noexcept
{
    std::vector<std::shared_ptr<T>> vec;
    auto it = _componentsMap.find(typeid(T));
    if (it != _componentsMap.end()) {
        vec.reserve(it->second.size());
        std::transform(it->second.begin(), it->second.end(), std::back_inserter(vec),
            [](std::shared_ptr<Component> shptr) {
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

template <class T, typename... Params>
inline std::shared_ptr<T> Component::Create(Params&&... args)
{
    auto component = std::make_shared<T>(args...);
    component->_typeIndex = typeid(T);
    return component;
    //component->GetComponentManager()->AddComponent(component);
    //return Reference(component, ComponentManager::CurrentManager());
}

template <typename T>
inline void Component::Replace(std::shared_ptr<T> oldComponent, std::shared_ptr<T> newComponent)
{
    _Replace(oldComponent, newComponent);
    auto componentsIt = _componentsMap.find(typeid(T));
    if (componentsIt != _componentsMap.end())
        std::replace(componentsIt->second.begin(), componentsIt->second.end(), oldComponent, newComponent);
    auto components { GetComponents() };
    std::replace(components.begin(), components.end(), oldComponent, newComponent);
}

inline auto Component::GetComponentsNbr() const noexcept
{
    return GetComponents().size();
}

inline std::unordered_set<std::shared_ptr<Component>> Component::GetComponents() const noexcept
{
    std::unordered_set<std::shared_ptr<Component>> components;
    for (const auto& type : _componentsMap) {
        components.insert(type.second.begin(), type.second.end());
    }
    return components;
}

inline std::unordered_set<std::shared_ptr<Component>> Component::GetComponents() noexcept
{
    std::unordered_set<std::shared_ptr<Component>> components;
    for (const auto& type : _componentsMap) {
        components.insert(type.second.begin(), type.second.end());
    }
    return components;
}

inline std::unordered_set<std::shared_ptr<Component>> Component::GetComponentsInChildren() const noexcept
{
    auto components = GetComponents();
    for (const auto& component : GetComponents()) {
        auto childComponents = component->GetComponentsInChildren();
        components.insert(childComponents.begin(), childComponents.end());
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

inline void Component::Replace(std::shared_ptr<Component> oldComponent, std::shared_ptr<Component> newComponent)
{
    _Replace(oldComponent, newComponent);
    for (auto& type : _componentsMap) {
        std::replace(type.second.begin(), type.second.end(), oldComponent, newComponent);
    }
}

inline int64_t Component::GetComponentIndex(std::type_index typeIndex, std::shared_ptr<Component> component) const noexcept
{
    auto componentsIt = _componentsMap.find(typeIndex);
    if (componentsIt == _componentsMap.end())
        return -1;
    auto componentIt = std::find(componentsIt->second.begin(), componentsIt->second.end(), component);
    if (componentIt == componentsIt->second.end())
        return -1;
    return std::distance(componentsIt->second.begin(), componentIt);
}

inline int64_t Component::AddComponent(std::type_index typeIndex, std::shared_ptr<Component> component) noexcept
{
    if (std::find(_componentsMap[typeIndex].begin(), _componentsMap[typeIndex].end(), component) == _componentsMap[typeIndex].end()) {
        _componentsMap[typeIndex].push_back(component);
    }
    return GetComponentIndex(typeIndex, component);
}
