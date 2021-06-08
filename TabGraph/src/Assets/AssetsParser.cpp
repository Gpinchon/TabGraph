/*
* @Author: gpinchon
* @Date:   2020-08-18 13:46:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-01 14:05:15
*/

#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>
#include <Debug.hpp>
#include <DispatchQueue.hpp>
#include <Event/EventsManager.hpp>

#include <assert.h>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <string>

static std::mutex s_parsingTaskMutex;
static std::condition_variable s_cv;
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

inline void NotifyLoaded(std::shared_ptr<Asset> asset)
{
    Event event;
    event.type = Event::Type::AssetLoaded;
    event.data = Event::Asset {
        asset->GetLoaded(),
        asset
    };
    EventsManager::PushEvent(event);
    s_parsingTaskMutex.lock();
    s_parsingAssets.erase(asset);
    s_parsingTaskMutex.unlock();
    s_cv.notify_all();
}

void AssetsParser::AddParsingTask(const ParsingTask& parsingTask)
{
    auto sharedAsset{ parsingTask.asset.lock() };
    {
        std::unique_lock<std::mutex> lock(s_parsingTaskMutex);
        if (parsingTask.type == ParsingTask::Type::Sync) {
            s_cv.wait(lock, [parsingTask] {
                return s_parsingAssets.count(parsingTask.asset) == 0;
            });
        }
        else if (s_parsingAssets.count(sharedAsset) > 0)
            return;
        s_parsingAssets.insert(sharedAsset);
    }
    if (sharedAsset->GetLoaded()) NotifyLoaded(sharedAsset);
    else if (parsingTask.type == ParsingTask::Type::Sync) {
        std::unique_lock<std::mutex> lock(sharedAsset->GetLock());
        AssetsParser::Parse(sharedAsset);
        NotifyLoaded(sharedAsset);
    } else {
        auto weakAsset { parsingTask.asset };
        DispatchQueue::ApplicationDispatchQueue().Dispatch([weakAsset] {
            auto sharedAsset { weakAsset.lock() };
            std::unique_lock<std::mutex> lock(sharedAsset->GetLock());
            AssetsParser::Parse(sharedAsset);
            NotifyLoaded(sharedAsset);
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
    auto parser { new AssetsParser(mimeType, parsingFunction) };
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
    auto& parser = _getParsers()[mime];
    return parser ? parser->_parsingFunction : nullptr;
}