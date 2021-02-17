#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"

Asset::Asset(const Uri& Uri) : _uri(Uri)
{
}

Asset::~Asset()
{
	if (GetLoading())
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

bool Asset::GetLoading()
{
	return _loadingFuture.valid();
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
	if (GetLoading())
		_loadingFuture.get();
	if (GetLoaded())
		return;
	_DoLoad();
}

void Asset::LoadAsync()
{
	if (GetLoading()) {
		if (_loadingFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
			return;
	}		
	if (GetLoaded()) {
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
		return;
	}
	_loadingFuture = std::async(std::launch::async, &Asset::_DoLoad, this);
}

void Asset::_DoLoad()
{
	if (!GetLoaded()) {
		AssetsParser::Parse(std::static_pointer_cast<Asset>(shared_from_this()));
		assert(GetLoaded());
	}
	if (GetLoaded())
		_onloaded(std::static_pointer_cast<Asset>(shared_from_this()));
}
