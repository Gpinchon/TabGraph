#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "Debug.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "SceneParser.hpp"
#include "Vgroup.hpp"
#include <iostream>
#include <filesystem>
#include <memory>
#define RAPIDJSON_NOEXCEPT_ASSERT(x)
#define RAPIDJSON_ASSERT(x) {if (x); else throw std::runtime_error("JSON error in " + std::string(__FILE__) + " at " + std::to_string(__LINE__));};
#include "rapidjson/document.h"

auto __gltfParser = SceneParser::Add("gltf", GLTF::Parse);

auto ParseCameras(const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Camera>> cameraVector;
	try {
		for (const auto &camera : document["cameras"].GetArray()) {
			std::cout << "found new camera" << std::endl;
			auto newCamera(Camera::Create("", 45));
			if (std::string(camera["type"].GetString()) == "perspective") {
				auto perspective(camera["perspective"].GetObject());
				try {
					newCamera->SetZfar(perspective["zfar"].GetFloat());
				}
				catch (std::exception &) {debugLog("No zfar property")}
				newCamera->SetFov(glm::degrees(perspective["yfov"].GetFloat()));
				newCamera->SetZnear(perspective["znear"].GetFloat());
			}
			cameraVector.push_back(newCamera);
		}
	}
	catch (std::exception &) {}
	return cameraVector;
}

auto ParseMaterials(const rapidjson::Document &document)
{
	debugLog("Start parsing materials");
	std::vector<std::shared_ptr<Material>> materialVector;
	try {
		auto materialIndex(0);
		for (const auto &materialValue : document["materials"].GetArray()) {
			auto material(Material::Create("Material " + std::to_string(materialIndex)));
			try {
				auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
				try {
					auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
					material->albedo.r = baseColor[0].GetFloat();
					material->albedo.g = baseColor[1].GetFloat();
					material->albedo.b = baseColor[2].GetFloat();
				}
				catch (std::exception &) {debugLog("No baseColorFactor property")}
				try {material->metallic = pbrMetallicRoughness["metallicFactor"].GetFloat();}
				catch (std::exception &) {debugLog("No metallicFactor property")}
				try {material->roughness = pbrMetallicRoughness["roughnessFactor"].GetFloat();}
				catch (std::exception &) {debugLog("No roughnessFactor property")}
			}
			catch(std::exception &) {debugLog("Not a pbrMetallicRoughness material")}
			materialVector.push_back(material);
		}
	}
	catch(std::exception &) {debugLog("No materials found")}
	debugLog("Done parsing materials");
	return materialVector;
}

auto parseMeshes(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing meshes");
	std::vector<std::shared_ptr<Mesh>> meshVector;
	auto meshesItr(document.FindMember("meshes"));
	if (meshesItr == document.MemberEnd()) {
		debugLog("No meshes found");
		return meshVector;
	}
	const auto materials(ParseMaterials(document));
	const auto &bufferViews(document["bufferViews"].GetArray());
	const auto &buffers(document["buffers"].GetArray());
	int meshIndex = 0;
	for (const auto &mesh : meshesItr->value.GetArray()) {
		debugLog("Found new mesh");
		auto currentMesh(Mesh::Create("Mesh " + std::to_string(meshIndex)));
		//currentMesh->AddMaterial(Material::Create(""));
		meshIndex++;
		for (const auto &primitive : mesh["primitives"].GetArray()) {
			auto vgroup(Vgroup::Create());
			if (auto material = primitive.FindMember("material"); material != primitive.MemberEnd())
			{
				//std::cout << materials.at(material->value.GetInt())->albedo.r << std::endl;
				//std::cout << materials.at(material->value.GetInt())->albedo.g << std::endl;
				//std::cout << materials.at(material->value.GetInt())->albedo.b << std::endl;
				currentMesh->AddMaterial(*std::find(materials.begin(), materials.end(), materials.at(material->value.GetInt())));
				vgroup->SetMaterialIndex(currentMesh->GetMaterialIndex(materials.at(material->value.GetInt())));
			}
			for (const auto &attribute : primitive["attributes"].GetObject()) {
				debugLog(attribute.name.GetString());
				/** Get the accessor */
				const auto &accessor(document["accessors"].GetArray()[attribute.value.GetInt()]);
				/** Get the bufferView from "bufferView" key in accessor */
				const auto &bufferView(bufferViews[accessor["bufferView"].GetInt()]);
				/** Get the buffer from "buffer" key in accessor */
				const auto &buffer(buffers[bufferView["buffer"].GetInt()]);
				auto bufferPath(std::filesystem::path(buffer["uri"].GetString()));
				if (!bufferPath.is_absolute())
					bufferPath = std::filesystem::path(path).parent_path()/bufferPath;
				debugLog(bufferPath);
				auto file(_wfopen(bufferPath.c_str(), L"rb"));
				auto accessorByteOffset(0);
				auto bufferViewByteOffset(0);
				try {
					accessorByteOffset = accessor["byteOffset"].GetInt();
				}
				catch (std::runtime_error &) { debugLog("No accessor byteOffset"); }
				try {
					bufferViewByteOffset = bufferView["byteOffset"].GetInt();
				}
				catch (std::runtime_error &) { debugLog("No bufferView byteOffset"); }
				switch(accessor["componentType"].GetInt()) {
					case GL_FLOAT :
					{
						if (std::string(accessor["type"].GetString()) == "VEC3") {
							std::vector<glm::vec3> vec3Vector(accessor["count"].GetInt());
							fseek(file, accessorByteOffset + bufferViewByteOffset, 0);
							fread(&vec3Vector.at(0), sizeof(vec3Vector.at(0)), vec3Vector.size(), file);
							if (std::string(attribute.name.GetString()) == "POSITION")
								vgroup->v = vec3Vector;
							else if (std::string(attribute.name.GetString()) == "NORMAL") {
								for (const auto &vn : vec3Vector) {
									vgroup->vn.push_back(VecToCVec4(vn));
								}
							}
						}
					}
					debugLog("Close file");
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
	debugLog("Done parsing meshes");
	return meshVector;
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
		try {
			glm::mat4 matrix;
			for (unsigned i(0); i < node["matrix"].GetArray().Size() && i < glm::uint(matrix.length() * 4); i++)
				matrix[i / 4][i % 4] = node["matrix"].GetArray()[i].GetFloat();
			newNode->SetNodeTransformMatrix(matrix);
		} catch (std::exception &) {}
		try {
			const auto &position(node["translation"].GetArray());
			newNode->SetPosition(glm::vec3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat()));
		} catch (std::exception &) {}
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
	rapidjson::ParseResult parseResult(document.Parse(file_to_str(path).c_str()));
	if (!parseResult) {
		debugLog("Invalid file !");
		return sceneVector;
	}
	auto nodes(parseNodes(path, document));
	auto scenes(document["scenes"].GetArray());
	int sceneIndex = 0;
	for (const auto &scene : scenes) {
		std::cout << "found scene" << std::endl;
		auto newScene(Scene::Create(std::to_string(sceneIndex)));
		for (const auto &node : scene["nodes"].GetArray()) {
			newScene->Add(nodes.at(node.GetInt()));
			std::cout << nodes.at(node.GetInt())->Name() << std::endl;
		}
		if (!newScene->Cameras().empty())
			newScene->SetCurrentCamera(*newScene->Cameras().begin());
		sceneVector.push_back(newScene);
		sceneIndex++;
	}
	return sceneVector;
}