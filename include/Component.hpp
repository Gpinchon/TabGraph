/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-12 10:12:01
*/

#pragma once

#include "Object.hpp"

#include <iostream>

class Component : public Object {
public:
    typedef std::unordered_map<std::type_index, std::shared_ptr<Component>> ComponentMap;
    Component()
        : Object() {};
    Component(const std::string& name)
        : Object(name) {};
    ~Component() = default;
    ComponentMap Components() const;
    template <typename T>
    void AddComponent(const std::shared_ptr<T>& component);
    template <typename T>
    std::shared_ptr<T> GetComponent();
    template <typename T>
    std::shared_ptr<T> GetComponent() const;
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
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->LoadCPU();
        }
    }
    /** Calls UnloadCPU for all sub Components */
    virtual void UnloadCPU() final
    {
        _UnloadCPU();
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->UnloadCPU();
        }
    }
    /** Calls LoadGPU for all sub Components */
    virtual void LoadGPU() final
    {
        _LoadGPU();
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->LoadGPU();
        }
    }
    /** Calls UnloadGPU for all sub Components */
    virtual void UnloadGPU() final
    {
        _UnloadGPU();
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->UnloadGPU();
        }
    }
    /** Calls UpdateCPU for all sub Components */
    virtual void UpdateCPU(float delta) final
    {
        if (NeedsUpdateCPU())
            _UpdateCPU(delta);
        SetNeedsUpdateCPU(false);
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->UpdateCPU(delta);
        }
    }
    /** Calls UpdateGPU for all sub Components */
    virtual void UpdateGPU(float delta) final
    {
        if (NeedsUpdateGPU())
            _UpdateGPU(delta);
        SetNeedsUpdateGPU(false);
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->UpdateGPU(delta);
        }
    }
    virtual void FixedUpdateCPU(float delta) final
    {
        _FixedUpdateCPU(delta);
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->FixedUpdateCPU(delta);
        }
    }
    virtual void FixedUpdateGPU(float delta) final
    {
        _FixedUpdateGPU(delta);
        for (const auto& component : _components) {
            if (component.second != nullptr)
                component.second->FixedUpdateGPU(delta);
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

//#include <iostream>

template <typename T>
void Component::AddComponent(const std::shared_ptr<T>& component)
{
    //std::cout << __FUNCTION__ << " " << Name() << " " << typeid(T).name() << " " << component->Name() << std::endl;
    _components[typeid(T)] = component;
}

template <typename T>
std::shared_ptr<T> Component::GetComponent()
{
    //std::cout << __FUNCTION__ << " " << Name() << " " << typeid(T).name() << std::endl;
    return std::static_pointer_cast<T>(_components[typeid(T)]);
}

template <typename T>
std::shared_ptr<T> Component::GetComponent() const
{
    //std::cout << __FUNCTION__ << " " << Name() << " " << typeid(T).name() << std::endl;
    return std::static_pointer_cast<T>(_components.at(typeid(T)));
}
