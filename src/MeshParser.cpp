/*
* @Author: gpi
* @Date:   2019-04-25 09:44:00
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 10:29:33
*/

#include <map>             // for map
#include <memory>          // for shared_ptr
#include <string>          // for string
#include "Debug.hpp"       // for debugLog
#include "MeshParser.hpp"  // for MeshParser, MeshParsingFunction

class Mesh;

std::map<std::string, MeshParser *> *MeshParser::_parsers = nullptr;//std::map<std::string, MeshParser *>();

MeshParser::MeshParser(const std::string &format, MeshParsingFunction parsingFunction)
    : _format(format), _parsingFunction(parsingFunction)
{
    debugLog(format);
}

MeshParser *MeshParser::add(const std::string &format, MeshParsingFunction parsingFunction)
{
    auto parser = new MeshParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::map<std::string, MeshParser *> &MeshParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<std::string, MeshParser *>;
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

MeshParsingFunction   MeshParser::_get(const std::string &format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}