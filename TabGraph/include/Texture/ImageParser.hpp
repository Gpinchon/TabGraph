/*
* @Author: gpinchon
* @Date:   2019-04-04 13:38:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:44:58
*/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <filesystem>

class Image;

typedef void (*ImageParsingFunction)(const std::shared_ptr<Image>&);

class ImageParser {
public:
    ImageParser(const std::filesystem::path& format, ImageParsingFunction);
    static ImageParser* Add(const std::filesystem::path& format, ImageParsingFunction);
    static std::shared_ptr<Image> Parse(const std::filesystem::path& path);
    static void Parse(std::shared_ptr<Image> image);

private:
    static ImageParsingFunction _get(const std::filesystem::path& format);
    static std::map<std::filesystem::path, ImageParser*>& _getParsers();
    static std::map<std::filesystem::path, ImageParser*>* _parsers;
    std::filesystem::path _format;
    ImageParsingFunction _parsingFunction;
};
