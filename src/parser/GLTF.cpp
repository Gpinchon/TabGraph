#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"
#include "Debug.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "SceneParser.hpp"
#include "TextureParser.hpp"
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

auto ParseTextures(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing textures");
	std::vector<std::shared_ptr<Texture>> textureVector;
	try {
		auto textureIndex(0);
		for (const auto &textureValue : document["images"].GetArray()) {
			std::string uri;
			try {
				auto texturePath(std::filesystem::path(textureValue["uri"].GetString()));
				if (!texturePath.is_absolute())
					texturePath = std::filesystem::path(path).parent_path()/texturePath;
				uri = texturePath.string();
			}
			catch(std::exception &) {debugLog("Texture " + std::to_string(textureIndex) + " has no Uri")}
			textureVector.push_back(TextureParser::parse("Texture " + std::to_string(textureIndex), uri));
		}
	}
	catch(std::exception &) {debugLog("No textures found")}
	debugLog("Done parsing textures");
	return textureVector;
}

auto ParseMaterials(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing materials");
	auto textureVector(ParseTextures(path, document));
	std::vector<std::shared_ptr<Material>> materialVector;
	try {
		auto materialIndex(0);
		for (const auto &materialValue : document["materials"].GetArray()) {
			auto material(Material::Create("Material " + std::to_string(materialIndex)));
			material->albedo = glm::vec3(1, 1, 1);
			try {
				auto textureObject(materialValue["normalTexture"].GetObject());
				material->set_texture_normal(textureVector.at(textureObject["index"].GetInt()));
			}
			catch (std::exception &) {debugLog("No roughnessFactor normalTexture")}
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
				try {
					auto textureObject(pbrMetallicRoughness["baseColorTexture"].GetObject());
					material->set_texture_albedo(textureVector.at(textureObject["index"].GetInt()));
				}
				catch (std::exception &) {debugLog("No roughnessFactor baseColorTexture")}
			}
			catch(std::exception &) {debugLog("Not a pbrMetallicRoughness material")}
			materialVector.push_back(material);
		}
	}
	catch(std::exception &) {debugLog("No materials found")}
	debugLog("Done parsing materials");
	return materialVector;
}

auto ParseBuffers(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing buffers");
	std::vector<std::shared_ptr<Buffer>> bufferVector;
	try {
		auto bufferIndex(0);
		for (const auto &bufferValue : document["buffers"].GetArray()) {
			auto buffer(Buffer::Create(bufferValue["byteLength"].GetFloat()));
			buffer->SetName("Buffer " + std::to_string(bufferIndex));
			try {
				auto bufferPath(std::filesystem::path(bufferValue["uri"].GetString()));
				if (!bufferPath.is_absolute())
					bufferPath = std::filesystem::path(path).parent_path()/bufferPath;
				buffer->SetUri(bufferPath.string());
			}
			catch(std::exception &) {debugLog(buffer->Name() + " has no Uri")}
			try {buffer->SetName(bufferValue["name"].GetString());}
			catch(std::exception &) {debugLog(buffer->Name() + " has no name")}
			bufferVector.push_back(buffer);
			bufferIndex++;
		}
	}
	catch(std::exception &) {debugLog("No buffers found")}
	debugLog("Done parsing buffers");
	return bufferVector;
}

auto ParseBufferViews(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing bufferViews");
	auto buffers(ParseBuffers(path, document));
	std::vector<std::shared_ptr<BufferView>> bufferViewVector;
	try {
		auto bufferViewIndex(0);
		for (const auto &bufferViewValue : document["bufferViews"].GetArray()) {
			auto bufferView(BufferView::Create(
				bufferViewValue["byteLength"].GetInt(),
				buffers.at(bufferViewValue["buffer"].GetInt())));
			bufferView->SetName("BufferView " + std::to_string(bufferViewIndex));
			try {bufferView->SetByteOffset(bufferViewValue["byteOffset"].GetInt());}
			catch(std::exception &) {debugLog(bufferView->Name() + " has no byteOffset")}
			try {bufferView->SetByteStride(bufferViewValue["byteStride"].GetInt());}
			catch(std::exception &) {debugLog(bufferView->Name() + " has no byteStride")}
			try {bufferView->SetTarget(bufferViewValue["target"].GetInt());}
			catch(std::exception &) {debugLog(bufferView->Name() + " has no target")}
			try {bufferView->SetName(bufferViewValue["name"].GetString());}
			catch(std::exception &) {debugLog(bufferView->Name() + " has no name")}
			bufferViewVector.push_back(bufferView);
			bufferViewIndex++;
		}
	}
	catch(std::exception &) {debugLog("No bufferViews found")}
	debugLog("Done parsing bufferViews");
	return bufferViewVector;
}

auto ParseBufferAccessors(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing bufferAccessors");
	auto bufferViews(ParseBufferViews(path, document));
	std::vector<std::shared_ptr<BufferAccessor>> bufferAccessorVector;
	try {
		auto bufferAccessorIndex(0);
		for (const auto &bufferAccessorValue : document["accessors"].GetArray()) {
			auto bufferAccessor(BufferAccessor::Create(
				bufferAccessorValue["componentType"].GetInt(),
				bufferAccessorValue["count"].GetInt(),
				bufferAccessorValue["type"].GetString()));
			bufferAccessor->SetName("BufferAccessor " + std::to_string(bufferAccessorIndex));
			try {bufferAccessor->SetBufferView(bufferViews.at(bufferAccessorValue["bufferView"].GetInt()));}
			catch(std::exception &) {debugLog(bufferAccessor->Name() + " has no bufferView")}
			try {bufferAccessor->SetByteOffset(bufferAccessorValue["byteOffset"].GetInt());}
			catch(std::exception &) {debugLog(bufferAccessor->Name() + " has no byteOffset")}
			try {bufferAccessor->SetNormalized(bufferAccessorValue["normalized"].GetBool());}
			catch(std::exception &) {debugLog(bufferAccessor->Name() + " has no normalized")}
			try {bufferAccessor->SetCount(bufferAccessorValue["count"].GetInt());}
			catch(std::exception &) {debugLog(bufferAccessor->Name() + " has no count")}
			try {bufferAccessor->SetName(bufferAccessorValue["name"].GetString());}
			catch(std::exception &) {debugLog(bufferAccessor->Name() + " has no name")}
			bufferAccessorVector.push_back(bufferAccessor);
			bufferAccessorIndex++;
		}
	}
	catch(std::exception &) {debugLog("No bufferAccessors found")}
	debugLog("Done parsing bufferAccessors");
	return bufferAccessorVector;
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
	auto defaultMaterial(Material::Create("defaultMaterial"));
	const auto accessors(ParseBufferAccessors(path, document));
	const auto materials(ParseMaterials(path, document));
	int meshIndex = 0;
	for (const auto &mesh : meshesItr->value.GetArray()) {
		debugLog("Found new mesh");
		auto currentMesh(Mesh::Create("Mesh " + std::to_string(meshIndex)));
		meshIndex++;
		for (const auto &primitive : mesh["primitives"].GetArray()) {
			auto vgroup(Vgroup::Create());
			if (auto material = primitive.FindMember("material"); material != primitive.MemberEnd())
			{
				currentMesh->AddMaterial(*std::find(materials.begin(), materials.end(), materials.at(material->value.GetInt())));
				vgroup->SetMaterialIndex(currentMesh->GetMaterialIndex(materials.at(material->value.GetInt())));
			}
			for (const auto &attribute : primitive["attributes"].GetObject())
				vgroup->SetAccessor(attribute.name.GetString(), accessors.at(attribute.value.GetInt()));
			try { vgroup->SetIndices(accessors.at(primitive["indices"].GetInt())); }
			catch(std::exception &) {debugLog("Vgroup " + vgroup->Name() + " has no indices")}
			try { vgroup->SetMode(primitive["mode"].GetInt()); }
			catch(std::exception &) {debugLog("Vgroup " + vgroup->Name() + " has no mode")}
			currentMesh->AddVgroup(vgroup);
		}
		if (currentMesh->GetMaterial(0) == nullptr)
			currentMesh->AddMaterial(defaultMaterial);
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
		newScene->SetUp(glm::vec3(0, 0, 1));
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