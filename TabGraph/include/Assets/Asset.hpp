#pragma once
#include "Component.hpp"
#include "Event/Signal.hpp"
#include "Assets/Uri.hpp"

#include <future>
#include <mutex>

class Asset : public Component {
	PROPERTY(std::string, AssetType, "");
	PROPERTY(Uri, Uri, );
public :
	Asset() = default;
	Asset(const Uri&);
	~Asset();
	bool GetLoaded();
	void SetLoaded(bool);
	Signal<std::shared_ptr<Asset>> &OnLoaded();
	/** @brief Loads the Image synchronously, must emit LoadedChanged(true) if loading was successful */
	virtual void Load();
	/**
	 * @brief Loads the Asset asynchronously -> returns immediatly.
	 * Emits LoadedChanged(true) if loading was successful.
	*/
	virtual void LoadAsync();

private:
	std::atomic<bool> _loaded{ false };
	std::mutex _loadingMutex{};
	Signal<std::shared_ptr<Asset>> _onloaded{};
	std::future<void> _loadingFuture{};
	void _DoLoad();

	// Hérité via Component
	virtual std::shared_ptr<Component> _Clone() override
	{
		//You cannot not clone an asset
		return std::static_pointer_cast<Component>(shared_from_this());
	}
};
