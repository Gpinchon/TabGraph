/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:23
*/

#include "Object.hpp"
#include "Debug.hpp"
#include <limits>
#include <random>

Object::Object()
{
    static uint64_t objNbr(0);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int64_t> dis(0, std::numeric_limits<int64_t>::max());
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
    debugLog(GetName());
}
