/*
* @Author: gpi
* @Date:   2019-04-25 09:40:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:31:44
*/

#pragma once

#include <map>
#include <memory>
#include <string>

class Scene;

typedef std::shared_ptr<Scene> (*MeshParsingFunction)(const std::string& name, const std::string& path);

class SceneParser {
public:
    static SceneParser* Add(const std::string& format, MeshParsingFunction);
    static std::shared_ptr<Scene> parse(const std::string& name, const std::string& path);

private:
    SceneParser() = delete;
    SceneParser(const std::string& format, MeshParsingFunction);
    static MeshParsingFunction _get(const std::string& format);
    static std::map<std::string, SceneParser*>& _getParsers();
    static std::map<std::string, SceneParser*>* _parsers;
    std::string _format;
    MeshParsingFunction _parsingFunction;
};