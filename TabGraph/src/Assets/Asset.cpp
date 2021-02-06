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

bool Asset::GetLoading()
{
	return _loadingThread.joinable();
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
	if (GetLoading()) // We're already loading
	{
		_loadingThread.join();
		assert(GetLoaded()); //there was an issue loading this asset...
	}
	_loadingThread = std::thread(&Asset::_DoLoad, this);
	_loadingThread.join();
}

void Asset::LoadAsync()
{
	if (GetLoading())
		return;
	_loadingThread = std::thread(&Asset::_DoLoad, this);
}

void Asset::_DoLoad()
{
	if (!GetLoaded())
		AssetsParser::Parse(std::static_pointer_cast<Asset>(shared_from_this()));
	if (GetLoaded())
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
}

std::shared_ptr<Component> Asset::_Clone()
{
	return std::static_pointer_cast<Component>(shared_from_this());
}
