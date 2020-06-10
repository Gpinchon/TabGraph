/*
* @Author: gpinchon
* @Date:   2020-06-08 13:30:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-09 23:16:02
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

private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> _components;
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
