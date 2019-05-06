/*
* @Author: gpi
* @Date:   2019-04-04 13:38:31
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:27:06
*/

#pragma once

#include <string>
#include <map>
#include <memory>

class Texture;

typedef std::shared_ptr<Texture> (*ParsingFunction)(const std::string&, const std::string&);

class TextureParser
{
public:
	static TextureParser			*add(const std::string &format, ParsingFunction);
	static std::shared_ptr<Texture> parse(const std::string& name, const std::string& path);
private:
	TextureParser() = delete;
	TextureParser(const std::string &format, ParsingFunction);
	static ParsingFunction			_get(const std::string &format);
	static std::map<std::string, TextureParser *> &_getParsers();
	static std::map<std::string, TextureParser *> *_parsers;
	std::string		_format;
	ParsingFunction _parsingFunction;
};