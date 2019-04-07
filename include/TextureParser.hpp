#pragma once

#include <string>
#include <map>
#include <memory>

class Texture;

typedef std::shared_ptr<Texture> (*ParsingFunction)(const std::string&, const std::string&);

class TextureParser
{
public:
	TextureParser() = delete;
	TextureParser(const std::string &format, ParsingFunction);
	static ParsingFunction	get(const std::string &format);
	static std::shared_ptr<Texture> parse(const std::string& name, const std::string& path);
private:
	ParsingFunction _parsingFunction;
	static std::map<std::string, TextureParser *> *_parsers;
};