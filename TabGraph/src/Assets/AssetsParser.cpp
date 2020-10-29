/*
* @Author: gpinchon
* @Date:   2020-08-18 13:46:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 21:13:58
*/

#include "Assets/AssetsParser.hpp" // for AssetsParser, AssetsParsingFunction
#include "Debug.hpp" // for debugLog

#include <filesystem>
#include <map> // for map
#include <memory> // for shared_ptr
#include <string> // for string

std::map<std::string, AssetsParser*>* AssetsParser::_parsers = nullptr; //std::map<std::string, AssetsParser *>();

AssetsParser::AssetsParser(const std::string& format, AssetsParsingFunction parsingFunction)
    : _format(format)
    , _parsingFunction(parsingFunction)
{
    debugLog(format);
}

AssetsParser* AssetsParser::Add(const std::string& format, AssetsParsingFunction parsingFunction)
{
    debugLog("Add Parser " + format);
    auto parser = new AssetsParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::map<std::string, AssetsParser*>& AssetsParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<std::string, AssetsParser*>;
    return *_parsers;
}

std::shared_ptr<AssetsContainer> AssetsParser::Parse(const std::filesystem::path path)
{
    auto format = std::filesystem::path(path).extension();
    debugLog(path);
    debugLog(format);
    auto parser = _get(format.string());
    debugLog(parser);
    return parser ? parser(path) : nullptr;
}

AssetsParsingFunction AssetsParser::_get(const std::string& format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}