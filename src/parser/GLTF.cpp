#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "rapidjson/document.h"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Vgroup.hpp"
#include <iostream>
#include <filesystem>
#include <memory>

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
				else if (std::string(attribute.name.GetString()) == "NORMAL")
				{
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
								for (const auto &vn : vec3Vector) {
									vgroup->vn.push_back(VecToCVec4(vn));
								}
							}
						}
					}
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

auto parseNodes(const std::string &path, const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Node>> nodeVector;
	auto meshes(parseMeshes(path, document));
	auto nodeItr(document.FindMember("nodes"));
	if (nodeItr == document.MemberEnd())
		return nodeVector;
	for (const auto &node : nodeItr->value.GetArray())
	{
		std::shared_ptr<Node> newNode;
		if (node.FindMember("mesh") != node.MemberEnd()) //This is a mesh
		{
			newNode = std::static_pointer_cast<Node>(meshes.at(node["mesh"].GetInt()));
		}
		else
		{
			newNode = Node::Create("");
		}
		if (node.FindMember("matrix") != node.MemberEnd())
		{
			glm::mat4 matrix;
			for (unsigned i(0); i < node["matrix"].GetArray().Size() && i < matrix.length() * 4; i++)
				matrix[i / 4][i % 4] = node["matrix"].GetArray()[i].GetFloat();
			newNode->SetNodeTransformMatrix(matrix);
		}
		nodeVector.push_back(newNode);
	}
	//Build parenting relationship
	int nodeIndex = 0;
	for (const auto &node : nodeItr->value.GetArray())
	{
		if (node.FindMember("children") != node.MemberEnd())
		{
			for (const auto &child : node["children"].GetArray())
				nodeVector.at(nodeIndex)->add_child(nodeVector.at(child.GetInt()));
		}
		nodeIndex++;
	}
	return nodeVector;
}

std::vector<Scene> GLTF::Parse(const std::string &path) {
	std::vector<Scene> sceneVector;
	rapidjson::Document document;
	document.Parse(file_to_str(path).c_str());
	auto nodes(parseNodes(path, document));
	auto scenes(document["scenes"].GetArray());
	int sceneIndex = 0;
	for (const auto &scene : scenes) {
		Scene newScene(std::to_string(sceneIndex));
		for (const auto &node : scene["nodes"].GetArray()) {
			newScene.Nodes().push_back(nodes.at(node.GetInt()));
			//newScene.Nodes().push_back(const value_type &__x);
			std::cout << node.GetInt() << std::endl;
		}
		sceneVector.push_back(newScene);
		sceneIndex++;
	}
	return sceneVector;
}