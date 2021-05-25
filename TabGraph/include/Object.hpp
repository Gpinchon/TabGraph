/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 14:28:18
*/

#pragma once

#include <Event/Signal.hpp>
#include <Property.hpp>

#include <memory> // for enable_shared_from_this, shared_ptr
#include <string> // for string

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/
class Object : public std::enable_shared_from_this<Object>, public Trackable {
    PROPERTY(int64_t, Id, reinterpret_cast<int64_t>(this));
    PROPERTY(std::string, Name, "");

public:
    Object();
    Object(const std::string& name);
    virtual ~Object();
};
