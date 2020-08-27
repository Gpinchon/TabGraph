/*
* @Author: gpinchon
* @Date:   2020-08-18 12:54:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 21:06:41
*/
#pragma once

#include <map>
#include <memory>
#include <string>

#include "Assets/AssetsContainer.hpp"

class Mesh;

typedef AssetsContainer (*AssetsParsingFunction)(const std::string& path);

class AssetsParser {
public:
    static AssetsParser* Add(const std::string& format, AssetsParsingFunction);
    static AssetsContainer Parse(const std::string& path);

private:
    AssetsParser() = delete;
    AssetsParser(const std::string& format, AssetsParsingFunction);
    static AssetsParsingFunction _get(const std::string& format);
    static std::map<std::string, AssetsParser*>& _getParsers();
    static std::map<std::string, AssetsParser*>* _parsers;
    std::string _format;
    AssetsParsingFunction _parsingFunction;
};