#pragma once
#include <Component.hpp>
#include <Assets/Uri.hpp>

#include <atomic>

class Asset : public Component {
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
