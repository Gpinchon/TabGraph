/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-09 09:56:50
*/

#include "Object.hpp"
#include "Debug.hpp"
#include <random>
#include <limits>

Object::Object()
{
    static uint64_t objNbr(0);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int64_t> dis(0, std::numeric_limits<int64_t>::max() );
    SetId(dis(gen)); //Generate unique ID
    SetName("Object_" + std::to_string(objNbr));
    objNbr++;
}

Object::Object(const std::string& name)
    : Object()
{
    SetName(name);
}

Object::~Object()
{
    debugLog(Name());
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