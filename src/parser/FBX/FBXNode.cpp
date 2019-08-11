/*
* @Author: gpinchon
* @Date:   2019-08-10 11:21:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 20:47:25
*/

#include "parser/FBX/FBXNode.hpp"
#include "parser/FBX/FBXObject.hpp"
#include "parser/FBX/FBXProperty.hpp"
#include <iostream>
#include <vector>

using namespace FBX;

std::shared_ptr<Node> Node::Create()
{
    auto ptr = std::shared_ptr<Node>(new Node());
    return ptr;
}

std::vector<std::shared_ptr<Node>>& Node::SubNodes(const std::string& name)
{
    return nodes[name];
}

std::shared_ptr<Node> Node::SubNode(const std::string& name)
{
    auto nodesVector = SubNodes(name);
    if (nodesVector.size() > 0)
        return nodesVector.at(0);
    return nullptr;
}

void Node::Print() const
{
    std::cout << "Node (\"" << Name() << "\", " << properties.size() << ") {\n";
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