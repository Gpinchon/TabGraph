/*
* @Author: gpinchon
* @Date:   2019-08-10 11:50:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 12:40:47
*/

#pragma once

#include "parser/FBX/FBXNode.hpp"
#include <string>

namespace FBX {
struct Header;

struct Document : public Node {
    Header* header;
    std::string path;
    void Print() const override;
    static Document* Parse(const std::string& path);

private:
    Document();
};
}
