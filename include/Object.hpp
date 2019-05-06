/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:27:32
*/

#pragma once

#include "GLIncludes.hpp"
#include <memory>
#include <string>
#include <vector>
/*#include <iostream>
#include <memory>
#include <string>*/

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

class Object : public std::enable_shared_from_this<Object> {
public:
    const std::string& name();
    void set_name(const std::string& name);

protected:
    Object();
    Object(const std::string& name);

private:
    std::string _name;
    static std::vector<std::shared_ptr<Object>> _objects;
};
