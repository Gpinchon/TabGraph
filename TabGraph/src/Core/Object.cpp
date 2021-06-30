/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 00:35:28
*/

#include <Core/Object.hpp>

#include <random>


namespace TabGraph::Core {
Object::Object()
{
    static auto s_objectNbr { 0u };
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    _id = dist(gen);
    _name = "Object_" + std::to_string(++s_objectNbr);
}
};
