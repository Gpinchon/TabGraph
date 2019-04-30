#include "GLIncludes.hpp"
#include "Mesh.hpp"
#include "MeshParser.hpp"

#include "Debug.hpp"

auto __genericTextureParser = MeshParser::add("generic", GenericTextureParser);

std::map<std::string, MeshParser *> *MeshParser::_parsers = nullptr;//std::map<std::string, MeshParser *>();

MeshParser::MeshParser(const std::string &format, ParsingFunction parsingFunction)
{
    debugLog(format);
    _parsingFunction = parsingFunction;
    if (_parsers == nullptr){
        _parsers = new std::map<std::string, MeshParser *>;
    }
    (*_parsers)[format] = this;
}

MeshParser *MeshParser::add(const std::string &format, ParsingFunction parsingFunction)
{
    auto parser = new MeshParser(format, parsingFunction);
    _getParsers()[format] = parser;
    return parser;
}

std::shared_ptr<MeshParser> MeshParser::parse(const std::string& name, const std::string& path)
{
	auto format = path.substr(path.find_last_of(".") + 1);
    debugLog(path);
    debugLog(format);
	auto parser = get(format);
    debugLog(parser);
	if (parser == nullptr)
		return (GenericTextureParser(name, path));
	return (parser(name, path));
}

ParsingFunction   MeshParser::get(const std::string &format)
{
    auto parser = _getParsers()[format];
    return parser ? parser->_parsingFunction : nullptr;
}