/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-12 10:12:01
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
    /** Attaches the specified component to the object */
    template <typename T>
    void AddComponent(const std::shared_ptr<T>& component)
    {
        if (component == nullptr)
            return;
        auto& components = _components[typeid(T)];
        auto componentIterator = std::find(components.begin(), components.end(), component);
        if (componentIterator == components.end())
            components.push_back(component);
    }
    /** Sets the first component attached to this object to component, adds it if component type is missing */
    template <typename T>
    void SetComponent(const std::shared_ptr<T>& component)
    {
        if (component == nullptr) {
            RemoveComponent(component);
        } else {
            auto& components = _components[typeid(T)];
            if (components.empty())
                AddComponent(component);
            else
                components.at(0) = component;
            //auto componentIterator = std::find(components.begin(), components.end(), component);
        }
    }
    /** Removes all the components of specified type from the object */
    template <typename T>
    void RemoveComponents()
    {
        _components[typeid(T)].clear();
    }
    /** Removes the specified component from the object */
    template <typename T>
    void RemoveComponent(const std::shared_ptr<T>& component)
    {
        auto& components = _components[typeid(T)];
        components.erase(std::remove(components.begin(), components.end(), component), components.end());
    }
    /** @return the first component of the specified type attached to the object, null if not found */
    template <typename T>
    std::shared_ptr<T> GetComponent()
    {
        auto& components = _components[typeid(T)];
        if (components.empty())
            return nullptr;
        return std::static_pointer_cast<T>(components.at(0));
    }
    /** @return the first component of the specified type attached to the object, null if not found */
    template <typename T>
    std::shared_ptr<T> GetComponent() const
    {
        auto& components = _components.at(typeid(T));
        if (components.empty())
            return nullptr;
        return std::static_pointer_cast<T>(components.at(0));
    }
    /** @return all components of the specified type attached to the object */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponents();
    /** @return all components of the specified type attached to the object */
    template <typename T>
    std::vector<std::shared_ptr<T>> GetComponents() const;
    virtual bool NeedsUpdateGPU() const final { return _needsUpdateGPU; };
    virtual void SetNeedsUpdateGPU(bool changed) final { _needsUpdateGPU = changed; };
    virtual bool NeedsUpdateCPU() const final { return _needsUpdateCPU; };
    virtual void SetNeedsUpdateCPU(bool changed) final { _needsUpdateCPU = changed; };
    virtual bool LoadedGPU() const final { return _loadedGPU; };
    virtual void SetLoadedGPU(bool loaded) final { _loadedGPU = loaded; };
    virtual bool LoadedCPU() const final { return _loadedCPU; };
    virtual void SetLoadedCPU(bool loaded) final { _loadedCPU = loaded; };
    /** Calls LoadCPU for all sub Components */
    virtual void LoadCPU() final
    {
        _LoadCPU();
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->LoadCPU();
        }
    }
    /** Calls UnloadCPU for all sub Components */
    virtual void UnloadCPU() final
    {
        _UnloadCPU();
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->UnloadCPU();
        }
    }
    /** Calls LoadGPU for all sub Components */
    virtual void LoadGPU() final
    {
        _LoadGPU();
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->LoadGPU();
        }
    }
    /** Calls UnloadGPU for all sub Components */
    virtual void UnloadGPU() final
    {
        _UnloadGPU();
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->UnloadGPU();
        }
    }
    /** Calls UpdateCPU for all sub Components */
    virtual void UpdateCPU(float delta) final
    {
        if (NeedsUpdateCPU())
            _UpdateCPU(delta);
        SetNeedsUpdateCPU(false);
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->UpdateCPU(delta);
        }
    }
    /** Calls UpdateGPU for all sub Components */
    virtual void UpdateGPU(float delta) final
    {
        if (NeedsUpdateGPU())
            _UpdateGPU(delta);
        SetNeedsUpdateGPU(false);
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->UpdateGPU(delta);
        }
    }
    virtual void FixedUpdateCPU(float delta) final
    {
        _FixedUpdateCPU(delta);
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->FixedUpdateCPU(delta);
        }
    }
    virtual void FixedUpdateGPU(float delta) final
    {
        _FixedUpdateGPU(delta);
        for (const auto& components : _components) {
            //if (component.second != nullptr)
            for (auto& component : components.second)
                component->FixedUpdateGPU(delta);
        }
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
    bool _needsUpdateGPU { false };
    bool _needsUpdateCPU { false };
    bool _loadedGPU { false };
    bool _loadedCPU { false };
};