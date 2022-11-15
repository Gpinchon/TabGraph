#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Assets/Uri.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset : public SG::Inherit<SG::Object, Asset> {
public:
    ///Parsing options for the various types of assets this could contain
    struct {
        struct {
            int32_t maximumResolution { -1 };
        } image;
        struct {
            bool compress { true };
            uint8_t compressionQuality { 255 };
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
    template<typename T>
    inline auto Get() {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : assets) {
            if (object->IsCompatible(typeid(T)))
                objects.push_back(std::static_pointer_cast<T>(object));
        }
        return objects;
    }
    inline auto GetByName(const std::string& name) {
        std::vector<std::shared_ptr<SG::Object>> objects;
        for (const auto& object : assets) {
            if (object->GetName() == name)
                objects.push_back(object);
        }
        return objects;
    }

    template<typename T>
    inline auto GetByName(const std::string& name) {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : Get<T>()) {
            if (object->GetName() == name)
                objects.push_back(object);
        }
        return objects;
    }
    inline void Add(std::shared_ptr<Asset> a_asset) {
        assets.insert(assets.end(), a_asset->assets.begin(), a_asset->assets.end());
    }
    std::vector<std::shared_ptr<SG::Object>> assets;

private:
    std::atomic<bool> _loaded { false };
    std::mutex _lock;
};
}
