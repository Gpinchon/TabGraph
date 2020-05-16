/*
* @Author: gpinchon
* @Date:   2019-08-10 11:50:30
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-08 11:12:28
*/

#pragma once

#include "Parser/FBX/FBXNode.hpp"
#include <string>

namespace FBX
{
struct Header;

struct Document : public Node
{
    virtual ~Document() = default;
    static Document *Parse(const std::string &path);
    Header *header;
    std::string path;
    void Print() const override;

private:
    Document();
};
} // namespace FBX
