/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:06:49
*/

#include "Object.hpp"

std::vector<std::shared_ptr<Object>> Object::_objects;

Object::Object()
{
    auto objectName = std::string("object_") + std::to_string(_objects.size());
    auto object = std::shared_ptr<Object>(new Object(objectName));
    _objects.push_back(object);
}

Object::Object(const std::string& name)
{
    set_name(name);
}

const std::string& Object::name()
{
    return (_name);
}

void Object::set_name(const std::string& name)
{
    _name = name;
}