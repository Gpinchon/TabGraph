#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>

Asset::Asset(const Uri& uri) : _Uri(uri)
{
}

Asset::~Asset()
{
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
	assert(GetLoaded());
}

void Asset::LoadAsync()
{
	AssetsParser::AddParsingTask({
		AssetsParser::ParsingTask::Type::Async,
		std::static_pointer_cast<Asset>(shared_from_this()),
	});
}
