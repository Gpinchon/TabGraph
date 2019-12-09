#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "rapidjson/document.h"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Vgroup.hpp"
#include <iostream>
#include <filesystem>

auto parseMeshes(const std::string &path, const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Mesh>> meshVector;
	auto meshesItr(document.FindMember("meshes"));
	if (meshesItr == document.MemberEnd())
		return meshVector;
	const auto &bufferViews(document["bufferViews"].GetArray());
	const auto &buffers(document["buffers"].GetArray());
	for (const auto &mesh : meshesItr->value.GetArray()) {
		auto currentMesh(Mesh::Create(""));
		currentMesh->AddMaterial(Material::Create(""));
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
						bufferPath = std::filesystem::path(path).parent_path()/bufferPath;
					std::cout << bufferPath << std::endl;
					auto file(_wfopen(bufferPath.c_str(), L"rb"));
					switch(accessor["componentType"].GetInt()) {
						case GL_FLOAT :
						{
							if (std::string(accessor["type"].GetString()) == "VEC3") {
								std::vector<glm::vec3> vec3Vector(accessor["count"].GetInt());
								fseek(file, accessor["byteOffset"].GetInt() + bufferView["byteOffset"].GetInt(), 0);
								fread(&vec3Vector.at(0), sizeof(vec3Vector.at(0)), vec3Vector.size(), file);
								vgroup->v = vec3Vector;
							}
						}
					}
					fclose(file);
				}
				std::cout << attribute.name.GetString() << std::endl;
			}
			auto indices(primitive.FindMember("indices"));
			if (indices != primitive.MemberEnd()) /** Vgroup has indices*/
			{ 
				/** Get the accessor */
				const auto &accessor(document["accessors"].GetArray()[indices->value.GetInt()]);
				/** Get the bufferView from "bufferView" key in accessor */
				const auto &bufferView(bufferViews[accessor["bufferView"].GetInt()]);
				/** Get the buffer from "buffer" key in accessor */
				const auto &buffer(buffers[bufferView["buffer"].GetInt()]);
				auto bufferPath(std::filesystem::path(buffer["uri"].GetString()));
				if (!bufferPath.is_absolute())
					bufferPath = std::filesystem::path(path).parent_path()/bufferPath;
				auto file(_wfopen(bufferPath.c_str(), L"rb"));
				switch(accessor["componentType"].GetInt()) {
					case GL_UNSIGNED_SHORT:
					{
						std::vector<unsigned short> ushortVector(accessor["count"].GetInt());
						fseek(file, accessor["byteOffset"].GetInt() + bufferView["byteOffset"].GetInt(), 0);
						fread(&ushortVector.at(0), sizeof(ushortVector.at(0)), ushortVector.size(), file);
						for (const auto &i : ushortVector)
							vgroup->i.push_back(i);
						std::cout << "GL_UNSIGNED_SHORT" << std::endl;
					}
				}
				fclose(file);
			}
			currentMesh->AddVgroup(vgroup);
		}
		meshVector.push_back(currentMesh);
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