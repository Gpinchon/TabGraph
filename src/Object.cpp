/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:57:25
*/

#include "Object.hpp"

std::vector<std::shared_ptr<Object>> Object::_objects;

/*Object::Object()
{
    auto objectName = std::string("object_") + std::to_string(_objects.size());
    auto object = std::shared_ptr<Object>(new Object(objectName));
    _objects.push_back(object);
}*/

Object::Object(const std::string& name)
{
    std::hash<std::string> hash_fn;
    SetName(name);
    SetId(hash_fn(Name()) + int64_t(this)); //Generate unique ID
}

std::shared_ptr<Object> Object::GetByName(const std::string& name)
{
    for (auto o : _objects) {
        if (name == o->Name())
            return (o);
    }
    return (nullptr);
}

std::shared_ptr<Object> Object::GetById(int64_t id)
{
    for (auto o : _objects) {
        if (id == o->Id())
            return (o);
    }
    return (nullptr);
}

void Object::Add(const std::shared_ptr<Object> object)
{
    _objects.push_back(object);
}

int64_t Object::Id() const
{
    return _id;
}

void Object::SetId(int64_t id)
{
    _id = id;
}

std::string Object::Name() const
{
    return (_name);
}

void Object::SetName(const std::string& name)
{
    _name = name;
}