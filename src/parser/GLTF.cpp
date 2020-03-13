#include "parser/GLTF.hpp"
#include "parser/InternalTools.hpp"
#include "Animation.hpp"
#include "AnimationSampler.hpp"
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"
#include "Debug.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "SceneParser.hpp"
#include "Texture2D.hpp"
#include "TextureParser.hpp"
#include "Vgroup.hpp"
#include <glm/ext.hpp>
#include <iostream>
#include <filesystem>
#include <memory>
#define RAPIDJSON_NOEXCEPT_ASSERT(x)
#define RAPIDJSON_ASSERT(x) {if (x); else throw std::runtime_error("JSON error in " + std::string(__FILE__) + " at " + std::to_string(__LINE__));};
#include "rapidjson/document.h"

auto __gltfParser = SceneParser::Add("gltf", GLTF::Parse);

struct TextureSampler
{
	std::map<std::string, GLenum> settings;
};

struct GLTFContainer
{
	std::vector<std::shared_ptr<BufferAccessor>> accessors;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Animation>> animations;
};

static inline auto ParseCameras(const rapidjson::Document &document)
{
	std::vector<std::shared_ptr<Camera>> cameraVector;
	try {
		auto cameraIndex(0);
		for (const auto &camera : document["cameras"].GetArray()) {
			auto newCamera(Camera::Create("Camera" + std::to_string(cameraIndex), 45));
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
			cameraIndex++;
		}
	}
	catch (std::exception &) {debugLog("No camera found")}
	return cameraVector;
}

static inline auto ParseTextureSamplers(const rapidjson::Document &document)
{
	std::vector<TextureSampler> samplerVector;
	try {
		for (const auto &sampler : document["samplers"].GetArray())
		{
			TextureSampler newSampler;
			for (rapidjson::Value::ConstMemberIterator setting = sampler.MemberBegin(); setting != sampler.MemberEnd(); setting++) {
				newSampler.settings[setting->name.GetString()] = setting->value.GetInt();
			}
			samplerVector.push_back(newSampler);
		}
	}
	catch (std::exception &) {debugLog("No sampler found")}
	return samplerVector;
}

static inline auto ParseTextures(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing textures");
	std::vector<std::shared_ptr<Texture2D>> textureVector;
	auto samplers(ParseTextureSamplers(document));
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
			auto texture(TextureParser::parse("Texture " + std::to_string(textureIndex), uri));
			try {
				/*
				"magFilter": 9729,
			    "minFilter": 9987,
			    "wrapS": 10497,
			    "wrapT": 10497
			    */
				auto sampler(samplers.at(textureValue["sampler"].GetInt()));
				if (sampler.settings["magFilter"] != 0)
					texture->set_parameteri(GL_TEXTURE_MAG_FILTER, sampler.settings["magFilter"]);
				if (sampler.settings["minFilter"] != 0)
					texture->set_parameteri(GL_TEXTURE_MIN_FILTER, sampler.settings["minFilter"]);
				if (sampler.settings["wrapS"] != 0)
					texture->set_parameteri(GL_TEXTURE_WRAP_S, sampler.settings["wrapS"]);
				if (sampler.settings["wrapT"] != 0)
					texture->set_parameteri(GL_TEXTURE_WRAP_T, sampler.settings["wrapT"]);
			}
			catch(std::exception &) {debugLog("Texture " + std::to_string(textureIndex) + " has no sampler")}
			textureVector.push_back(texture);
			textureIndex++;
		}
	}
	catch(std::exception &) {debugLog("No textures found")}
	debugLog("Done parsing textures");
	return textureVector;
}

static inline auto ParseMaterials(const std::string &path, const rapidjson::Document &document)
{
	debugLog("Start parsing materials");
	auto textureVector(ParseTextures(path, document));
	std::vector<std::shared_ptr<Material>> materialVector;
	try {
		auto materialIndex(0);
		for (const auto &materialValue : document["materials"].GetArray()) {
			auto material(Material::Create("Material " + std::to_string(materialIndex)));
			material->albedo = glm::vec3(1, 1, 1);
			material->uv_scale = glm::vec2(1, -1);
			try {
				auto baseColor(materialValue["emissiveFactor"].GetArray());
				material->emitting.r = baseColor[0].GetFloat();
				material->emitting.g = baseColor[1].GetFloat();
				material->emitting.b = baseColor[2].GetFloat();
				material->alpha = baseColor[3].GetFloat();
			}
			catch (std::exception &) {debugLog("No emissiveFactor property")}
			try {
				auto textureObject(materialValue["normalTexture"].GetObject());
				material->set_texture_normal(textureVector.at(textureObject["index"].GetInt()));
			}
			catch (std::exception &) {debugLog("No normalTexture property")}
			try {
				auto textureObject(materialValue["emissiveTexture"].GetObject());
				material->set_texture_emitting(textureVector.at(textureObject["index"].GetInt()));
			}
			catch (std::exception &) {debugLog("No normalTexture property")}
			try {
				auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
				try {
					auto textureObject(pbrMetallicRoughness["metallicRoughnessTexture"].GetObject());
					material->set_texture_metallicRoughness(textureVector.at(textureObject["index"].GetInt()));
				}
				catch (std::exception &) {debugLog("No metallicRoughnessTexture property")}
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
				catch (std::exception &) {debugLog("No baseColorTexture property")}
			}
			catch(std::exception &) {debugLog("Not a pbrMetallicRoughness material")}
			materialVector.push_back(material);
			materialIndex++;
		}
	}
	catch(std::exception &) {debugLog("No materials found")}
	debugLog("Done parsing materials");
	return materialVector;
}

static inline auto ParseBuffers(const std::string &path, const rapidjson::Document &document)
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

static inline auto ParseBufferViews(const std::string &path, const rapidjson::Document &document)
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

static inline auto ParseBufferAccessors(const std::string &path, const rapidjson::Document &document)
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

static inline auto ParseMeshes(const rapidjson::Document &document, const GLTFContainer &container)
{
	debugLog("Start parsing meshes");
	std::vector<std::shared_ptr<Mesh>> meshVector;
	auto meshesItr(document.FindMember("meshes"));
	if (meshesItr == document.MemberEnd()) {
		debugLog("No meshes found");
		return meshVector;
	}
	auto defaultMaterial(Material::Create("defaultMaterial"));
	//const auto materials(ParseMaterials(path, document));
	int meshIndex = 0;
	for (const auto &mesh : meshesItr->value.GetArray()) {
		debugLog("Found new mesh");
		auto currentMesh(Mesh::Create("Mesh " + std::to_string(meshIndex)));
		meshIndex++;
		for (const auto &primitive : mesh["primitives"].GetArray()) {
			auto vgroup(Vgroup::Create());
			if (auto material = primitive.FindMember("material"); material != primitive.MemberEnd())
			{
				currentMesh->AddMaterial(*std::find(container.materials.begin(), container.materials.end(), container.materials.at(material->value.GetInt())));
				vgroup->SetMaterialIndex(currentMesh->GetMaterialIndex(container.materials.at(material->value.GetInt())));
			}
			for (const auto &attribute : primitive["attributes"].GetObject()) {
				auto attributeName(std::string(attribute.name.GetString()));
				auto accessor(container.accessors.at(attribute.value.GetInt()));
				if (accessor->GetBufferView()->Target() == 0) {
					/*if (attributeName == "POSITION" ||
						attributeName == "NORMAL" ||
						attributeName == "TANGENT" ||
						attributeName == "TEXCOORD_0" ||
						attributeName == "TEXCOORD_1")*/
					accessor->GetBufferView()->SetTarget(GL_ARRAY_BUFFER);
				}
				vgroup->SetAccessor(attributeName, accessor);
			}
			try {
				auto accessor(container.accessors.at(primitive["indices"].GetInt()));
				if (accessor->GetBufferView()->Target() == 0)
					accessor->GetBufferView()->SetTarget(GL_ELEMENT_ARRAY_BUFFER);
				vgroup->SetIndices(accessor);
			}
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

static inline auto ParseNodes(const rapidjson::Document &document, const GLTFContainer &container)
{
	std::vector<std::shared_ptr<Node>> nodeVector;
	/*const auto accessors(ParseBufferAccessors(path, document));
	auto container.meshes(ParseMeshes(path, document, accessors));
	auto container.cameras(ParseCameras(document));*/
	auto nodeItr(document.FindMember("nodes"));
	if (nodeItr == document.MemberEnd())
		return nodeVector;
	int nodeIndex = 0;
	for (const auto &node : nodeItr->value.GetArray())
	{
		std::shared_ptr<Node> newNode;
		if (node.FindMember("mesh") != node.MemberEnd()) //This is a mesh
		{
			newNode = std::dynamic_pointer_cast<Node>(Mesh::Create(container.meshes.at(node["mesh"].GetInt())));
			newNode->SetName("MeshNode " + std::to_string(nodeIndex));
			std::cout << "Found new mesh " << newNode->Name() << " Use count " << newNode.use_count() << std::endl;
		}
		else if (node.FindMember("camera") != node.MemberEnd())
		{
			newNode = std::dynamic_pointer_cast<Node>(Camera::Create(container.cameras.at(node["camera"].GetInt())));
			newNode->SetName("CameraNode " + std::to_string(nodeIndex));
			std::cout << "Found new camera " << newNode->Name() << " Use count " << newNode.use_count() << std::endl;
		}
		else
			newNode = Node::Create("Node " + std::to_string(nodeIndex));
		try { newNode->SetName(node["name"].GetString()); }
		catch (std::exception &) {}
		try {
			glm::mat4 matrix;
			for (unsigned i(0); i < node["matrix"].GetArray().Size() && i < glm::uint(matrix.length() * 4); i++)
				matrix[i / 4][i % 4] = node["matrix"].GetArray()[i].GetFloat();
			newNode->SetNodeTransformMatrix(matrix);
		} catch (std::exception &) {}
		try {
			const auto &position(node["translation"].GetArray());
			glm::vec3 positionVec3;
			positionVec3[0] = position[0].GetFloat();
			positionVec3[1] = position[1].GetFloat();
			positionVec3[2] = position[2].GetFloat();
			newNode->SetPosition(positionVec3);
		} catch (std::exception &) {}
		try {
			const auto &rotation(node["rotation"].GetArray());
			glm::quat rotationQuat;
			rotationQuat[0] = rotation[0].GetFloat();
			rotationQuat[1] = rotation[1].GetFloat();
			rotationQuat[2] = rotation[2].GetFloat();
			rotationQuat[3] = rotation[3].GetFloat();
			newNode->SetRotation(glm::normalize(rotationQuat));
		} catch (std::exception &) {}
		try {
			const auto &scale(node["scale"].GetArray());
			newNode->SetScale(glm::vec3(scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat()));
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
				nodeVector.at(nodeIndex)->AddChild(nodeVector.at(child.GetInt()));
				std::cout << "Node parenting " << nodeVector.at(nodeIndex)->Name() << " -> " << nodeVector.at(child.GetInt())->Name() << std::endl;
				std::cout << "Child node use count " << nodeVector.at(child.GetInt()).use_count() << std::endl;
			}
		}
		nodeIndex++;
	}
	return nodeVector;
}

static inline auto ParseAnimations(const rapidjson::Document &document, const GLTFContainer &container) {
	std::vector<std::shared_ptr<Animation>> animations;
	try {
		for (const auto &animation : document["animations"].GetArray()) {
			auto newAnimation(Animation::Create());
			try { newAnimation->SetName(animation["name"].GetString()); }
			catch (std::exception &) { debugLog("No name property found"); }
			for (const auto &sampler : animation["samplers"].GetArray()) {
				auto samplerInput(container.accessors.at(sampler["input"].GetInt()));
				auto samplerOutput(container.accessors.at(sampler["output"].GetInt()));
				debugLog(samplerOutput->Type());
				auto newSampler(AnimationSampler(samplerInput, samplerOutput));
				try {
					std::string interpolation(sampler["interpolation"].GetString());
					if (interpolation == "LINEAR")
						newSampler.SetInterpolation(AnimationSampler::Linear);
					else if (interpolation == "STEP")
						newSampler.SetInterpolation(AnimationSampler::Step);
					else if (interpolation == "CUBICSPLINE")
						newSampler.SetInterpolation(AnimationSampler::CubicSpline);
				}
				catch (std::exception &) { debugLog("No interpolation property found"); }
				newAnimation->AddSampler(newSampler);
			}
			for (const auto &channel : animation["channels"].GetArray()) {
				AnimationChannel newChannel;
				try {
					auto &target = (channel["target"]);
					std::string path(target["path"].GetString());
					newChannel.SetSamplerIndex(channel["sampler"].GetInt());
					newChannel.SetTarget(container.nodes.at(target["node"].GetInt()));
					if (path == "translation")
						newChannel.SetPath(AnimationChannel::Translation);
					else if (path == "rotation")
						newChannel.SetPath(AnimationChannel::Rotation);
					else if (path == "scale")
						newChannel.SetPath(AnimationChannel::Scale);
					else if (path == "weights")
						newChannel.SetPath(AnimationChannel::Weights);
				}
				catch (std::exception &) { debugLog("No target property found"); }
				newAnimation->AddChannel(newChannel);
			}
			animations.push_back(newAnimation);
		}
	}
	catch (std::exception &) { debugLog("No animations found"); }
	return animations;
}

std::vector<std::shared_ptr<Scene>> GLTF::Parse(const std::string &path) {
	std::vector<std::shared_ptr<Scene>> sceneVector;
	rapidjson::Document document;
	rapidjson::ParseResult parseResult(document.Parse(file_to_str(path).c_str()));
	if (!parseResult) {
		debugLog("Invalid file !");
		return sceneVector;
	}
	GLTFContainer container;
	container.cameras = ParseCameras(document);
	container.accessors = ParseBufferAccessors(path, document);
	container.materials = ParseMaterials(path, document);
	container.meshes = ParseMeshes(document, container);
	container.nodes = ParseNodes(document, container);
	container.animations = ParseAnimations(document, container);
	auto scenes(document["scenes"].GetArray());
	int sceneIndex = 0;
	for (const auto &scene : scenes) {
		std::cout << "found scene" << std::endl;
		auto newScene(Scene::Create(std::to_string(sceneIndex)));
		newScene->SetUp(glm::vec3(0, 1, 0));
		for (const auto &node : scene["nodes"].GetArray()) {
			newScene->Add(container.nodes.at(node.GetInt()));
			std::cout << container.nodes.at(node.GetInt())->Name() << std::endl;
		}
		if (!newScene->Cameras().empty())
			newScene->SetCurrentCamera(*newScene->Cameras().begin());
		for (const auto animation : container.animations) {
			std::cout << animation << std::endl;
			newScene->Add(animation);
			/*for (const auto channel : animation->GetChannels()) {
				if (newScene->GetNode(channel.Target())) {
					std::cout << "Found Node" << std::endl;
					newScene->Add(animation);
					break;
				}
			}*/
		}
		sceneVector.push_back(newScene);
		sceneIndex++;
	}
	return sceneVector;
}