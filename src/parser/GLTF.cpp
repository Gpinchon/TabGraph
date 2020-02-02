#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "rapidjson/document.h"
#include "Material.hpp"
#include "Camera.hpp"
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
	int meshIndex = 0;
	for (const auto &mesh : meshesItr->value.GetArray()) {
		auto currentMesh(Mesh::Create("Mesh " + std::to_string(meshIndex)));
		currentMesh->AddMaterial(Material::Create(""));
		meshIndex++;
		for (const auto &primitive : mesh["primitives"].GetArray()) {
			auto vgroup(Vgroup::Create());
			for (const auto &attribute : primitive["attributes"].GetObject()) {
				std::cout << attribute.name.GetString() << std::endl;
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

std::vector<std::shared_ptr<Camera>> ParseCameras(const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Camera>> cameraVector;
	auto cameraItr(document.FindMember("cameras"));
	if (cameraItr == document.MemberEnd())
		return cameraVector;
	for (const auto &cameraValue : cameraItr->value.GetArray()) {
		std::cout << "found new camera" << std::endl;
		auto camera(Camera::Create("", 45));
		if (std::string(cameraValue["type"].GetString()) == "perspective") {
			auto perspective(cameraValue["perspective"].GetObject());
			//camera->SetFov(perspective["yfov"].GetFloat());
			camera->SetZfar(perspective["zfar"].GetFloat());
			camera->SetZnear(perspective["znear"].GetFloat());
		}
		cameraVector.push_back(camera);
	}
	return cameraVector;
}

auto parseNodes(const std::string &path, const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Node>> nodeVector;
	auto meshes(parseMeshes(path, document));
	auto cameras(ParseCameras(document));
	auto nodeItr(document.FindMember("nodes"));
	if (nodeItr == document.MemberEnd())
		return nodeVector;
	int nodeIndex = 0;
	for (const auto &node : nodeItr->value.GetArray())
	{
		std::shared_ptr<Node> newNode;
		if (node.FindMember("mesh") != node.MemberEnd()) //This is a mesh
		{
			newNode = std::dynamic_pointer_cast<Node>(Mesh::Create(meshes.at(node["mesh"].GetInt())));
			newNode->SetName("MeshNode " + std::to_string(nodeIndex));
			std::cout << "Found new mesh " << newNode->Name() << " Use count " << newNode.use_count() << std::endl;
		}
		else if (node.FindMember("camera") != node.MemberEnd())
		{
			newNode = std::dynamic_pointer_cast<Node>(Camera::Create(cameras.at(node["camera"].GetInt())));
			newNode->SetName("CameraNode " + std::to_string(nodeIndex));
			std::cout << "Found new camera " << newNode->Name() << " Use count " << newNode.use_count() << std::endl;
		}
		else
			newNode = Node::Create("Node " + std::to_string(nodeIndex));
		if (node.FindMember("matrix") != node.MemberEnd())
		{
			glm::mat4 matrix;
			for (unsigned i(0); i < node["matrix"].GetArray().Size() && i < matrix.length() * 4; i++)
				matrix[i / 4][i % 4] = node["matrix"].GetArray()[i].GetFloat();
			newNode->SetNodeTransformMatrix(matrix);
		}
		if (node.FindMember("translation") != node.MemberEnd())
		{
			const auto &position(node["translation"].GetArray());
			newNode->SetPosition(glm::vec3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat()));
		}
		nodeVector.push_back(newNode);
		nodeIndex++;
	}
	//Build parenting relationship
	nodeIndex = 0;
	for (const auto &node : nodeItr->value.GetArray())
	{
		if (node.FindMember("children") != node.MemberEnd())
		{
			for (const auto &child : node["children"].GetArray()) {
				nodeVector.at(nodeIndex)->add_child(nodeVector.at(child.GetInt()));
				std::cout << "Node parenting " << nodeVector.at(nodeIndex)->Name() << " -> " << nodeVector.at(child.GetInt())->Name() << std::endl;
				std::cout << "Child node use count " << nodeVector.at(child.GetInt()).use_count() << std::endl;
			}
		}
		nodeIndex++;
	}
	return nodeVector;
}

std::vector<std::shared_ptr<Scene>> GLTF::Parse(const std::string &path) {
	std::vector<std::shared_ptr<Scene>> sceneVector;
	rapidjson::Document document;
	document.Parse(file_to_str(path).c_str());
	auto nodes(parseNodes(path, document));
	auto scenes(document["scenes"].GetArray());
	int sceneIndex = 0;
	for (const auto &scene : scenes) {
		std::shared_ptr<Scene> newScene(new Scene(std::to_string(sceneIndex)));
		for (const auto &node : scene["nodes"].GetArray()) {
			newScene->Nodes().push_back(nodes.at(node.GetInt()));
			std::cout << nodes.at(node.GetInt())->Name() << std::endl;
		}
		sceneVector.push_back(newScene);
		sceneIndex++;
	}
	return sceneVector;
}