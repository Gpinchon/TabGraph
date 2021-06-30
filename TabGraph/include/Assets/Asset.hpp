#pragma once

#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Property.hpp>
#include <Assets/Uri.hpp>
#include <Config.hpp>

#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Scene;
}
namespace TabGraph::Assets {
class BinaryData;
class Image;
}

////////////////////////////////////////////////////////////////////////////////
//Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset : public Core::Inherit<Core::Object, Asset> {
public:
	///Parsing options for the various types of assets this could contain
	struct {
		struct {
			int32_t maximumResolution{ Config::Global().Get("ImagesMaximumResolution", -1) };
		} image;
		struct {
			bool compress{ bool(Config::Global().Get("TexturesCompressed", 1)) };
			uint8_t compressionQuality{ uint8_t(Config::Global().Get("TexturesCompressionQuality", 0.25f) * 255) };
		} texture;
	} parsingOptions;
	PROPERTY(std::string, AssetType, "");
	PROPERTY(Uri, Uri, );
public:
	Asset() = default;
	Asset(const Uri&);
	Asset(const Asset&) = delete;
	inline bool GetLoaded()
	{
		return _loaded.load();
	}
	inline void SetLoaded(bool loaded)
	{
		_loaded.store(loaded);
	}
	inline std::mutex& GetLock()
	{
		return _lock;
	}

	std::vector<std::shared_ptr<Nodes::Scene>> scenes;
	std::vector<std::shared_ptr<BinaryData>> binaryDatas;
	std::vector<std::shared_ptr<Image>> images;

private:
	std::atomic<bool> _loaded{ false };
	std::mutex _lock;
};
}
