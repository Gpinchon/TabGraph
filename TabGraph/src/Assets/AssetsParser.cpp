/*
* @Author: gpinchon
* @Date:   2020-08-18 13:46:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 21:13:58
*/

#include "Assets/AssetsParser.hpp" // for AssetsParser, AssetParsingFunction
#include "Assets/Asset.hpp"
#include "Debug.hpp" // for debugLog
#include <filesystem>
#include <map> // for map
#include <memory> // for shared_ptr
#include <string> // for string

std::map<AssetsParser::MimeType, AssetsParser*>* AssetsParser::_parsers = nullptr;
std::map<AssetsParser::FileExtension, AssetsParser::MimeType>* AssetsParser::_mimesExtensions = nullptr;

AssetsParser::AssetsParser(const MimeType& mimeType, ParsingFunction parsingFunction)
    : _mimeType(mimeType)
    , _parsingFunction(parsingFunction)
{
    //debugLog(mimeType);
}

AssetsParser::MimeType AssetsParser::GetMimeFromExtension(const FileExtension& extension)
{
    return _getMimesExtensions()[extension];
}

AssetsParser* AssetsParser::Add(const MimeType& mimeType, ParsingFunction parsingFunction)
{
    //debugLog("Add Parser " + mimeType);
    auto parser = new AssetsParser(mimeType, parsingFunction);
    _getParsers()[mimeType] = parser;
    
    return parser;
}

AssetsParser::MimeExtensionPair AssetsParser::AddMimeExtension(const MimeType& mime, const FileExtension& extension)
{
    _getMimesExtensions()[extension] = mime;
    return MimeExtensionPair(mime, extension);
}

std::map<AssetsParser::MimeType, AssetsParser*>& AssetsParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<MimeType, AssetsParser*>;
    return *_parsers;
}

#include <assert.h>

std::map<AssetsParser::FileExtension, AssetsParser::MimeType>& AssetsParser::_getMimesExtensions()
{
    if (_mimesExtensions == nullptr)
        _mimesExtensions = new std::map<FileExtension, MimeType>;
    return *_mimesExtensions;
}

void AssetsParser::Parse(std::shared_ptr<Asset> asset)
{
    assert(asset != nullptr);
    auto uriScheme = asset->GetUri().GetScheme();
    MimeType mime;
    if (uriScheme == "data")
        mime = DataUri(asset->GetUri()).GetMime();
    else
        mime = GetMimeFromExtension(AssetsParser::FileExtension(asset->GetUri().GetPath()).extension());
    //debugLog(mime);
    auto parser = _get(mime);
    if (parser != nullptr)
        parser(asset);
}

AssetsParser::ParsingFunction AssetsParser::_get(const MimeType& mime)
{
    auto parser = _getParsers()[mime];
    return parser ? parser->_parsingFunction : nullptr;
}