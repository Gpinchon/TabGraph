/*
* @Author: gpinchon
* @Date:   2019-08-10 11:20:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 12:40:43
*/

#pragma once

#include <map>
#include <string>
#include <vector>

namespace FBX {
class Property;

struct Node {
    /** @return a vector containing all nodes with this name */
    std::vector<Node*>& SubNodes(const std::string& name);
    /** @return the first node named with this name */
    Node* SubNode(const std::string& name);
    /** @return the property at this index */
    Property& Property(const size_t index);
    virtual void Print() const;
    std::string name { "" };
    std::vector<class Property*> properties;
    std::map<std::string, std::vector<Node*>> nodes;
};
}
