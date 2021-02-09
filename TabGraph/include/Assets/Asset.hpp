#pragma once
#include "Component.hpp"
#include "Event/Signal.hpp"
#include "Assets/Uri.hpp"
#include <thread>

class Asset : public Component {
	PROPERTY(std::string, AssetType, "");
public :
	Asset() = default;
	Asset(const Uri&);
	~Asset();
	void SetUri(const Uri& uri);
	Uri GetUri() const;
	bool GetLoading();
	std::atomic<bool>& GetLoaded();
	void SetLoaded(bool);
	Signal<std::shared_ptr<Asset>> &OnLoaded();
	virtual void Load();
	virtual void LoadAsync();

private:
	Uri _uri{};
	std::atomic<bool> _loaded{ false };
	Signal<std::shared_ptr<Asset>> _onloaded{};
	std::thread _loadingThread{};
	void _DoLoad();

	// Hérité via Component
	virtual std::shared_ptr<Component> _Clone() override
	{
		//throw std::runtime_error("You cannot not clone an asset");
		return std::static_pointer_cast<Component>(shared_from_this());
	}
};
