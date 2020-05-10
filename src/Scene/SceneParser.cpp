/*
* @Author: gpi
* @Date:   2019-04-25 09:44:00
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:34:23
*/

#include "Scene/SceneParser.hpp" // for SceneParser, SceneParsingFunction
#include "Debug.hpp" // for debugLog
#include <map> // for map
#include <memory> // for shared_ptr
#include <string> // for string

class Scene;

std::map<std::string, SceneParser*>* SceneParser::_parsers = nullptr;

SceneParser::SceneParser(const std::string& format, SceneParsingFunction parsingFunction)
    : _format(format)
    , _parsingFunction(parsingFunction)
{
    debugLog(format);
}

SceneParser* SceneParser::Add(const std::string& format, SceneParsingFunction parsingFunction)
{
    auto parser = new SceneParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::map<std::string, SceneParser*>& SceneParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<std::string, SceneParser*>;
    return *_parsers;
}

std::vector<std::shared_ptr<Scene>> SceneParser::Parse(const std::string& path)
{
    auto format = path.substr(path.find_last_of(".") + 1);
    debugLog(path);
    debugLog(format);
    auto parser = _get(format);
    debugLog(parser);
    return parser ? parser(path) : std::vector<std::shared_ptr<Scene>>();
}

SceneParsingFunction SceneParser::_get(const std::string& format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}