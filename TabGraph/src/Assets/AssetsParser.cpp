/*
* @Author: gpinchon
* @Date:   2020-08-18 13:46:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-07 15:39:40
*/

#include <Assets/AssetsParser.hpp>
#include <Assets/Asset.hpp>
#include <Debug.hpp>
#include <Event/EventsManager.hpp>
#include <DispatchQueue.hpp>

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <assert.h>
#include <set>

constexpr auto ParsingThreads = 4;

static std::mutex s_parsingTaskMutex;
static std::set<std::weak_ptr<Asset>, std::owner_less<>> s_parsingAssets;

std::map<AssetsParser::MimeType, std::unique_ptr<AssetsParser>>& _getParsers()
{
    static std::map<AssetsParser::MimeType, std::unique_ptr<AssetsParser>> s_parsers;
    return s_parsers;
}

std::map<AssetsParser::FileExtension, AssetsParser::MimeType>& _getMimesExtensions()
{
    static std::map<AssetsParser::FileExtension, AssetsParser::MimeType> s_mimesExtensions;
    return s_mimesExtensions;
}

inline void PushEvent(std::shared_ptr<Asset> asset) {
    Event event;
    event.type = Event::Type::AssetLoaded;
    event.data = Event::Asset{
        asset->GetLoaded(),
        asset
    };
    EventsManager::PushEvent(event);
}

void AssetsParser::AddParsingTask(const ParsingTask& parsingTask)
{
    auto sharedAsset{ parsingTask.asset.lock() };
    s_parsingTaskMutex.lock();
    auto inserted{ s_parsingAssets.insert(sharedAsset).second };
    s_parsingTaskMutex.unlock();
    if (!inserted) return;
    if (sharedAsset->GetLoaded())
        PushEvent(sharedAsset);
    else if (parsingTask.type == ParsingTask::Type::Sync) {
        AssetsParser::Parse(sharedAsset);
        PushEvent(sharedAsset);
    }
    else {
        auto weakAsset{ parsingTask.asset };
        DispatchQueue::ApplicationDispatchQueue().Dispatch([weakAsset] {
            auto sharedAsset{ weakAsset.lock() };
            AssetsParser::Parse(sharedAsset);
            PushEvent(sharedAsset);
            s_parsingTaskMutex.lock();
            s_parsingAssets.erase(sharedAsset);
            s_parsingTaskMutex.unlock();
            });
    }
}

AssetsParser::AssetsParser(const MimeType& mimeType, ParsingFunction parsingFunction)
    : _mimeType(mimeType)
    , _parsingFunction(parsingFunction)
{
}

AssetsParser::MimeType AssetsParser::GetMimeFromExtension(const FileExtension& extension)
{
    return _getMimesExtensions()[extension];
}

AssetsParser& AssetsParser::Add(const MimeType& mimeType, ParsingFunction parsingFunction)
{
    auto parser{ new AssetsParser(mimeType, parsingFunction) };
    _getParsers()[mimeType].reset(parser);
    return *parser;
}

AssetsParser::MimeExtensionPair AssetsParser::AddMimeExtension(const MimeType& mime, const FileExtension& extension)
{
    _getMimesExtensions()[extension] = mime;
    return MimeExtensionPair(mime, extension);
}

bool AssetsParser::Parse(std::shared_ptr<Asset> asset)
{
    assert(asset != nullptr);
    auto uriScheme = asset->GetUri().GetScheme();
    MimeType mime;
    if (uriScheme == "data")
        mime = DataUri(asset->GetUri()).GetMime();
    else
        mime = GetMimeFromExtension(AssetsParser::FileExtension(asset->GetUri().GetPath()).extension());
    auto parser = _get(mime);
    if (parser != nullptr) {
        parser(asset);
        return true;
    }
    return false;
}

AssetsParser::ParsingFunction AssetsParser::_get(const MimeType& mime)
{
    auto &parser = _getParsers()[mime];
    return parser ? parser->_parsingFunction : nullptr;
}