#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"

Asset::Asset(const Uri& Uri) : _uri(Uri)
{
}

Asset::~Asset()
{
	if (_loadingFuture.valid())
		_loadingFuture.get();
}

void Asset::SetUri(const Uri& uri)
{
	_uri = uri;
}

Uri Asset::GetUri() const
{
	return _uri;
}

bool Asset::GetLoaded()
{
	return _loaded.load();
}

void Asset::SetLoaded(bool loaded)
{
	_loaded = loaded;
}

Signal<std::shared_ptr<Asset>>& Asset::OnLoaded()
{
	return _onloaded;
}

void Asset::Load()
{
	std::lock_guard<std::mutex> guard(_loadingMutex);
	if (_loadingFuture.valid())
		_loadingFuture.get();
	if (GetLoaded()) {
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
		return;
	}
	AssetsParser::Parse(std::static_pointer_cast<Asset>(shared_from_this()));
	assert(GetLoaded());
	_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
}

void Asset::LoadAsync()
{
	if (!_loadingMutex.try_lock())
		return;
	if (_loadingFuture.valid()) {
		if (_loadingFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
			_loadingMutex.unlock();
			return;
		}
	}		
	if (GetLoaded()) {
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
		_loadingMutex.unlock();
		return;
	}
	_loadingFuture = std::async(std::launch::async, &Asset::_DoLoad, this);
	_loadingMutex.unlock();
}

void Asset::_DoLoad()
{
	std::lock_guard<std::mutex> guard(_loadingMutex);
	if (!GetLoaded()) {
		AssetsParser::Parse(std::static_pointer_cast<Asset>(shared_from_this()));
		assert(GetLoaded());
	}
	if (GetLoaded())
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
}
