/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-09 10:09:35
*/

#pragma once

//#include "Tools/memory.hpp"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <memory> // for enable_shared_from_this, shared_ptr
#include <string> // for string
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector> // for vector

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

class Object : public std::enable_shared_from_this<Object> {
public:
    Object();
    Object(const std::string& name);
    int64_t Id() const;
    void SetId(int64_t);
    std::string Name() const;
    void SetName(const std::string& name);
    virtual ~Object();

private:
    std::string _name { "" };
    int64_t _id { reinterpret_cast<int64_t>(this) };
};
