/*
* @Author: gpinchon
* @Date:   2019-08-10 11:21:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 12:40:42
*/

#include "parser/FBX/FBXNode.hpp"
#include "parser/FBX/FBXProperty.hpp"
#include <iostream>
#include <vector>

using namespace FBX;

std::vector<Node*>& Node::SubNodes(const std::string& name)
{
    return nodes[name];
}

Node* Node::SubNode(const std::string& name)
{
    auto nodesVector = SubNodes(name);
    if (nodesVector.size() > 0)
        return nodesVector.at(0);
    return nullptr;
}

void Node::Print() const
{
    std::cout << "Node (\"" << name << "\", " << properties.size() << ") {\n";
    for (const auto property : properties)
        property->Print();
    for (const auto subNodes : nodes) {
        for (const auto& subNode : subNodes.second)
            subNode->Print();
    }
    std::cout << "};" << std::endl;
}

Property& Node::Property(const size_t index)
{
    return *properties.at(index);
}