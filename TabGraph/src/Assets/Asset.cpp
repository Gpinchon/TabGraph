#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>

Asset::Asset(const Uri& uri) : _Uri(uri)
{
	static auto s_assetNbr = 0u;
	SetName("Asset_" + std::to_string(++s_assetNbr));
}

bool Asset::GetLoaded()
{
	return _loaded.load();
}

void Asset::SetLoaded(bool loaded)
{
	_loaded.store(loaded);
}

void Asset::Load()
{
	AssetsParser::AddParsingTask({
		AssetsParser::ParsingTask::Type::Sync,
		std::static_pointer_cast<Asset>(shared_from_this()),
	});
	//TODO figure out why I need to do this ?!
	while (!GetLoaded()) {}
	if (!GetLoaded())
		throw std::runtime_error(GetName() + " not loaded");
}

void Asset::LoadAsync()
{
	AssetsParser::AddParsingTask({
		AssetsParser::ParsingTask::Type::Async,
		std::static_pointer_cast<Asset>(shared_from_this()),
	});
}
