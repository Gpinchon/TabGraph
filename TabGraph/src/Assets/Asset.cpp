#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>

Asset::Asset(const Uri& uri) : _Uri(uri)
{
	static auto s_assetNbr = 0u;
	SetName("Asset_" + std::to_string(++s_assetNbr));
}
