/*
* @Author: gpi
* @Date:   2019-04-25 09:44:00
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:21:38
*/

#include "Mesh/MeshParser.hpp" // for MeshParser, MeshParsingFunction
#include "Debug.hpp" // for debugLog
#include <map> // for map
#include <memory> // for shared_ptr
#include <string> // for string

class Mesh;

std::map<std::string, MeshParser*>* MeshParser::_parsers = nullptr; //std::map<std::string, MeshParser *>();

MeshParser::MeshParser(const std::string& format, MeshParsingFunction parsingFunction)
    : _format(format)
    , _parsingFunction(parsingFunction)
{
    debugLog(format);
}

MeshParser* MeshParser::Add(const std::string& format, MeshParsingFunction parsingFunction)
{
    auto parser = new MeshParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::map<std::string, MeshParser*>& MeshParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<std::string, MeshParser*>;
    return *_parsers;
}

std::shared_ptr<Mesh> MeshParser::parse(const std::string& name, const std::string& path)
{
    auto format = path.substr(path.find_last_of(".") + 1);
    debugLog(path);
    debugLog(format);
    auto parser = _get(format);
    debugLog(parser);
    return parser ? parser(name, path) : nullptr;
}

MeshParsingFunction MeshParser::_get(const std::string& format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}