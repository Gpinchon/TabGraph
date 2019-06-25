/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 10:32:02
*/

#pragma once

#include <memory>  // for enable_shared_from_this, shared_ptr
#include <string>  // for string
#include <vector>  // for vector

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

class Object : public std::enable_shared_from_this<Object> {
public:
    const std::string& name();
    void set_name(const std::string& name);
    virtual ~Object() = default;

protected:
    Object();
    Object(const std::string& name);

private:
    std::string _name;
    static std::vector<std::shared_ptr<Object>> _objects;
};
