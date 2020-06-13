/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-12 10:12:01
*/

#pragma once

#include "Object.hpp"

class Component : public Object {
public:
    Component()
        : Object() {};
    Component(const std::string& name)
        : Object(name) {};
    ~Component() = default;
    template <typename T>
    void AddComponent(const std::shared_ptr<T>& component);
    template <typename T>
    std::shared_ptr<T> GetComponent();
    template <typename T>
    std::shared_ptr<T> GetComponent() const;
    bool NeedsUpdateGPU() const { return _needsUpdateGPU; };
    void SetNeedsUpdateGPU(bool changed) { _needsUpdateGPU = changed; };
    bool NeedsUpdateCPU() const { return _needsUpdateCPU; };
    void SetNeedsUpdateCPU(bool changed) { _needsUpdateCPU = changed; };
    bool LoadedGPU() const { return _loadedGPU; };
    void SetLoadedGPU(bool loaded) { _loadedGPU = loaded; };
    bool LoadedCPU() const { return _loadedCPU; };
    void SetLoadedCPU(bool loaded) { _loadedCPU = loaded; };
    virtual void LoadCPU() = 0;
    virtual void UnloadCPU() = 0;
    virtual void LoadGPU() = 0;
    virtual void UnloadGPU() = 0;
    virtual void UpdateCPU() = 0;
    virtual void UpdateGPU() = 0;

private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> _components;
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
