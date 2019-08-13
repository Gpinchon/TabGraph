/*
* @Author: gpinchon
* @Date:   2019-08-10 20:03:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 15:13:05
*/

#pragma once

#include <map>
#include <memory>
#include <vector>

namespace FBX {
struct Object {
    std::string Name() const;
    void SetName(const std::string&);
    virtual void Print() const;

    /** Searches for the corresponding object located at this id */
    static std::shared_ptr<Object> Get(int64_t id);
    /** Adds the object to specified id so it can be searched */
    static void Add(int64_t id, std::shared_ptr<Object>);

private:
    static std::map<int64_t, std::shared_ptr<Object>> s_objects;
    std::string _name { "" };
};
}