#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"

Asset::Asset(const Uri& Uri) : _uri(Uri)
{
}

Asset::~Asset()
{
	if (_loadingThread.joinable())
		_loadingThread.join();
}

void Asset::SetUri(const Uri& uri)
{
	_uri = uri;
}

Uri Asset::GetUri() const
{
	return _uri;
}

std::atomic<bool>& Asset::GetLoading()
{
	return _loading;
}

std::atomic<bool>& Asset::GetLoaded()
{
	return _loaded;
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
	bool shouldBeLoaded = false;
	while (GetLoading()) { //we're already loading from an other thread, wait
		shouldBeLoaded = true;
	}
	if (shouldBeLoaded)
		assert(GetLoaded()); //there was an issue loading this asset
	if (GetLoaded())
		return;
	_DoLoad();
}

void Asset::LoadAsync()
{
	if (GetLoading())
		return;
	if (GetLoaded()) {
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
		return;
	}
	_loadingThread = std::thread(&Asset::_DoLoad, this);
}

void Asset::_DoLoad()
{
	_loading = true;
	if (!GetLoaded())
		AssetsParser::Parse(std::static_pointer_cast<Asset>(shared_from_this()));
	_loading = false;
	if (GetLoaded())
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
}
