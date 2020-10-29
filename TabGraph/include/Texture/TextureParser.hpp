/*
* @Author: gpi
* @Date:   2019-04-04 13:38:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:16:55
*/

#pragma once

#include <map>
#include <memory>
#include <string>

class Texture2D;

typedef std::shared_ptr<Texture2D> (*TextureParsingFunction)(const std::string&, const std::string&);

class TextureParser {
public:
    TextureParser(const std::string& format, TextureParsingFunction);
    static TextureParser* Add(const std::string& format, TextureParsingFunction);
    static std::shared_ptr<Texture2D> parse(const std::string& name, const std::string& path);

private:
    static TextureParsingFunction _get(const std::string& format);
    static std::map<std::string, TextureParser*>& _getParsers();
    static std::map<std::string, TextureParser*>* _parsers;
    std::string _format;
    TextureParsingFunction _parsingFunction;
};
