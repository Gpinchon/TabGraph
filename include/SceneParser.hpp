/*
* @Author: gpi
* @Date:   2019-04-25 09:40:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:31:44
*/

#pragma once

#include <map>
#include <memory>
#include <vector>
#include <string>

class Scene;

typedef std::vector<std::shared_ptr<Scene>> (*SceneParsingFunction)(const std::string& path);

class SceneParser {
public:
    static SceneParser* Add(const std::string& format, SceneParsingFunction);
    static std::vector<std::shared_ptr<Scene>> Parse(const std::string& path);

private:
    SceneParser() = delete;
    SceneParser(const std::string& format, SceneParsingFunction);
    static SceneParsingFunction _get(const std::string& format);
    static std::map<std::string, SceneParser*>& _getParsers();
    static std::map<std::string, SceneParser*>* _parsers;
    std::string _format;
    SceneParsingFunction _parsingFunction;
};