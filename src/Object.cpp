/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:57:25
*/

#include "Object.hpp"

Object::Object(const std::string& name)
{
    std::hash<std::string> hash_fn;
    SetName(name);
    SetId(hash_fn(Name()) + int64_t(this)); //Generate unique ID
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