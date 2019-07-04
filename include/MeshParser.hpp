/*
* @Author: gpi
* @Date:   2019-04-25 09:40:04
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-07 14:51:52
*/

#pragma once

#include <map>
#include <memory>
#include <string>

class Mesh;

typedef std::shared_ptr<Mesh> (*MeshParsingFunction)(const std::string& name, const std::string& path);

class MeshParser {
public:
    static MeshParser* add(const std::string& format, MeshParsingFunction);
    static std::shared_ptr<Mesh> parse(const std::string& name, const std::string& path);

private:
    MeshParser() = delete;
    MeshParser(const std::string& format, MeshParsingFunction);
    static MeshParsingFunction _get(const std::string& format);
    static std::map<std::string, MeshParser*>& _getParsers();
    static std::map<std::string, MeshParser*>* _parsers;
    std::string _format;
    MeshParsingFunction _parsingFunction;
};