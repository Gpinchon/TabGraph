#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "rapidjson/document.h"
#include "Mesh.hpp"
#include "Vgroup.hpp"
#include <iostream>
#include <filesystem>

auto parseMeshes(const std::string &path, const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Mesh>> meshVector;
	auto meshesItr(document.FindMember("meshes"));
	if (meshesItr != document.MemberEnd()) {
		const auto &bufferViews(document["bufferViews"].GetArray());
		const auto &buffers(document["buffers"].GetArray());
		for (const auto &mesh : meshesItr->value.GetArray()) {
			for (const auto &primitive : mesh["primitives"].GetArray()) {
				auto vgroup(Vgroup::Create());
				for (const auto &attribute : primitive["attributes"].GetObject()) {
					if (std::string(attribute.name.GetString()) == "POSITION") {
						/** Get the accessor */
						const auto &accessor(document["accessors"].GetArray()[attribute.value.GetInt()]);
						/** Get the bufferView from "bufferView" key in accessor */
						const auto &bufferView(bufferViews[accessor["bufferView"].GetInt()]);
						/** Get the buffer from "buffer" key in accessor */
						const auto &buffer(buffers[bufferView["buffer"].GetInt()]);
						auto bufferPath(std::filesystem::path(buffer["uri"].GetString()));
						if (!bufferPath.is_absolute())
							bufferPath = std::filesystem::path(path).root_directory()/bufferPath;
						auto file(_wfopen(bufferPath.c_str(), L"rb"));
						std::vector<std::byte> byteVector;
						fread(byteVector.data(), 1, buffer["byteLength"].GetInt(), file);

					}
					std::cout << attribute.name.GetString() << std::endl;
				}
			}
			
		}
	}
	return meshVector;
}

std::vector<Scene> GLTF::Parse(const std::string &path) {
	std::vector<Scene> scenesVector;
	rapidjson::Document document;
	document.Parse(file_to_str(path).c_str());
	auto meshes(parseMeshes(path, document));
	auto nodes(document["nodes"].GetArray());
	auto scenes(document["scenes"].GetArray());
	int sceneIndex = 0;
	for (const auto &scene : scenes) {
		Scene newScene(std::to_string(sceneIndex));
		auto sceneNodes(scene["nodes"].GetArray());
		for (const auto &sceneMember : scene.GetObject()) {
			std::string memberName(sceneMember.name.GetString());
		}
		sceneIndex++;
	}
	return scenesVector;
}