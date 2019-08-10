/*
* @Author: gpinchon
* @Date:   2019-08-10 11:26:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 12:40:42
*/

#include "parser/FBX/FBXProperty.hpp"
#include <iostream>

using namespace FBX;

void Property::Print() const
{
    std::cout << "  Property(";
    switch (typeCode) {
    case ('C'):
        std::cout << "bool, " << std::get<Byte>(data);
        break;
    case ('Y'):
        std::cout << "int16_t, " << std::get<int16_t>(data);
        break;
    case ('I'):
        std::cout << "int32_t, " << std::get<int32_t>(data);
        break;
    case ('L'):
        std::cout << "int64_t, " << std::get<int64_t>(data);
        break;
    case ('F'):
        std::cout << "float, " << std::get<float>(data);
        break;
    case ('D'):
        std::cout << "double, " << std::get<double>(data);
        break;
    case ('S'):
        std::cout << "string, \"" << std::string(Array(*this)) << "\"";
        break;
    case ('R'):
        std::cout << "Byte *, " << std::get<Array>(data).length;
        break;
    case ('b'):
        std::cout << "bool *, " << std::get<Array>(data).length;
        break;
    case ('i'):
        std::cout << "int32_t *, " << std::get<Array>(data).length;
        break;
    case ('l'):
        std::cout << "int64_t *, " << std::get<Array>(data).length;
        break;
    case ('f'):
        std::cout << "float *, " << std::get<Array>(data).length;
        break;
    case ('d'):
        std::cout << "double *, " << std::get<Array>(data).length;
        break;
    }
    std::cout << ");\n";
}
