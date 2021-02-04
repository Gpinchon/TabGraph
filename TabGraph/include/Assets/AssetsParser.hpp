/*
* @Author: gpinchon
* @Date:   2020-08-18 12:54:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-01 08:38:35
*/
#pragma once

#include <functional>
#include <memory>
#include <string>     // for string
#include <filesystem>
#include <map>

class Asset;

class AssetsParser {
public:
    using FileExtension = std::filesystem::path;
    using MimeType = std::string;
    using ParsingFunction = std::function<void(std::shared_ptr<Asset>)>;
    using MimeExtensionPair = std::pair<MimeType, FileExtension>;
    /**
     * @brief Returns the MIME type if managed
     * @param  the file extension to get the MIME type for
     * @return the corresponding MIME type or "" if not managed
    */
    static MimeType GetMimeFromExtension(const FileExtension&);
    static AssetsParser* Add(const MimeType&, ParsingFunction);
    static MimeExtensionPair AddMimeExtension(const MimeType& mime, const FileExtension& extension);
    static void Parse(std::shared_ptr<Asset> asset);

private:
    AssetsParser() = delete;
    AssetsParser(const MimeType&, ParsingFunction);
    static ParsingFunction _get(const MimeType&);
    static std::map<MimeType, AssetsParser*>& _getParsers();
    static std::map<MimeType, AssetsParser*>* _parsers;
    static std::map<FileExtension, MimeType>& _getMimesExtensions();
    static std::map<FileExtension, MimeType>* _mimesExtensions;
    MimeType _mimeType;
    ParsingFunction _parsingFunction;
};