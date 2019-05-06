/*
* @Author: gpi
* @Date:   2019-04-25 09:40:04
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:27:41
*/

#pragma once

#include <string>
#include <map>
#include <memory>

class Mesh;

typedef std::shared_ptr<Mesh> (*ParsingFunction)(const std::string&name, const std::string&path);

class MeshParser
{
public:
	static MeshParser				*add(const std::string &format, ParsingFunction);
	static std::shared_ptr<Mesh>	parse(const std::string& name, const std::string& path);
private:
	MeshParser() = delete;
	MeshParser(const std::string &format, ParsingFunction);
	static ParsingFunction			_get(const std::string &format);
	static std::map<std::string, MeshParser *> &_getParsers();
	static std::map<std::string, MeshParser *> *_parsers;
	std::string		_format;
	ParsingFunction _parsingFunction;
};