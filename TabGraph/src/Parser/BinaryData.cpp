/*
* @Author: gpinchon
* @Date:   2021-02-03 17:50:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-03 17:50:55
*/

#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"
#include "Assets/BinaryData.hpp"
#include "Assets/Uri.hpp"

#include <fstream>

void ParseBinaryData(const std::shared_ptr<Asset>& asset);

auto bufferMime { AssetsParser::AddMimeExtension("application/octet-stream", ".bin") };
auto bufferParser { AssetsParser::Add("application/octet-stream", ParseBinaryData) };

void ParseBinaryData(const std::shared_ptr<Asset>& asset)
{
    std::shared_ptr<BinaryData> binaryData;
    {
        auto uri { asset->GetUri() };
        std::vector<std::byte> data;
        if (uri.GetScheme() == "data") {
            data = DataUri(uri).Decode();
        } else {
            data.resize(std::filesystem::file_size(uri.GetPath()));
            std::ifstream file(uri.GetPath(), std::ios::binary);
            file.read(reinterpret_cast<char*>(data.data()), data.size());
        }
        binaryData = Component::Create<BinaryData>(data);
    }
    asset->SetComponent(binaryData);
    asset->SetAssetType(BinaryData::AssetType);
    asset->SetLoaded(true);
}