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

#include <filesystem>
#include <map> // for map
#include <memory> // for shared_ptr
#include <string> // for string
#include <assert.h>
#include <chrono>
#include <set>
#include <queue>
#include <thread>
#include <mutex>

constexpr auto ParsingThreads = 4;

struct DispatchQueue {
    DispatchQueue(size_t threadsCount = 1)
        : _threads(threadsCount)
    {
        for (auto& thread : _threads)
            thread = std::thread(&DispatchQueue::_DispatchThreadHandler, this);
    }
    ~DispatchQueue() {
        _running = false;
        _cv.notify_all();
        for (auto& thread : _threads)
            if (thread.joinable()) thread.join();
    }
    void Dispatch(std::function<void(void)> fun) {
        std::unique_lock<std::mutex> lock(_lock);
        _queue.push(fun);
        lock.unlock();
        _cv.notify_one();
    }
private :
    void _DispatchThreadHandler(void) {
        std::unique_lock<std::mutex> lock(_lock);
        while (_running) {
            _cv.wait(lock, [this] {
                return _queue.size() || !_running;
            });
            if (_queue.empty()) continue;
            auto fun{ std::move(_queue.front()) };
            _queue.pop();
            lock.unlock();
            fun();
            lock.lock();
        }
    }
    bool _running{ true };
    std::vector<std::thread> _threads;
    std::mutex _lock;
    std::condition_variable _cv;
    std::queue<std::function<void(void)>> _queue;
};

std::map<AssetsParser::MimeType, std::unique_ptr<AssetsParser>>* AssetsParser::_parsers = nullptr;
std::map<AssetsParser::FileExtension, AssetsParser::MimeType>* AssetsParser::_mimesExtensions = nullptr;
static std::mutex s_parsingTaskMutex;
static std::set<std::shared_ptr<Asset>> s_parsingAssets;
static DispatchQueue s_dispatchQueue(ParsingThreads);

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
        s_dispatchQueue.Dispatch([weakAsset] {
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

std::map<AssetsParser::MimeType, std::unique_ptr<AssetsParser>>& AssetsParser::_getParsers()
{
    if (_parsers == nullptr)
        _parsers = new std::map<MimeType, std::unique_ptr<AssetsParser>>;
    return *_parsers;
}

std::map<AssetsParser::FileExtension, AssetsParser::MimeType>& AssetsParser::_getMimesExtensions()
{
    if (_mimesExtensions == nullptr)
        _mimesExtensions = new std::map<FileExtension, MimeType>;
    return *_mimesExtensions;
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