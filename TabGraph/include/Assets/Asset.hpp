#pragma once
#include <Component.hpp>
#include <Assets/Uri.hpp>
#include <Config.hpp>

#include <atomic>

class Asset : public Component {
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
public :
	Asset() = default;
	Asset(const Uri&);
	bool GetLoaded();
	void SetLoaded(bool);
	/** @brief Loads the Asset synchronously, must emit an Event of type Event::AssetLoaded on completion */
	virtual void Load();
	/**
	 * @brief Loads the Asset asynchronously -> returns immediatly.
	 * Emit an Event of type Event::AssetLoaded on completion.
	*/
	virtual void LoadAsync();

private:
	std::atomic<bool> _loaded{ false };
	virtual std::shared_ptr<Component> _Clone() override
	{
		//You cannot not clone an asset
		return std::static_pointer_cast<Component>(shared_from_this());
	}
};
