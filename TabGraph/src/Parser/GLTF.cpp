/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 17:15:22
*/

#include "Parser/GLTF.hpp"
#include "Animation/Animation.hpp"
#include "Animation/AnimationSampler.hpp"
#include "Assets/Assetscontainer.hpp"
#include "Assets/AssetsParser.hpp"
#include "Buffer/Buffer.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferView.hpp"
#include "Camera/Camera.hpp"
#include "Debug.hpp"
#include "Material/Material.hpp"
#include "Mesh/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/MeshSkin.hpp"
#include "Parser/InternalTools.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Texture2D.hpp"
#include "Texture/TextureParser.hpp"
#include "Transform.hpp"

#include <fstream>
#include <filesystem>
#include <glm/ext.hpp>
#include <iostream>
#include <memory>

#define RAPIDJSON_NOEXCEPT_ASSERT(x)
#define RAPIDJSON_ASSERT(x)                                                                                         \
    {                                                                                                               \
        if (x)                                                                                                      \
            ;                                                                                                       \
        else                                                                                                        \
            throw std::runtime_error("JSON error in " + std::string(__FILE__) + " at " + std::to_string(__LINE__)); \
    };
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

auto __gltfParser = AssetsParser::Add(".gltf", GLTF::Parse);

struct TextureSampler {
    std::map<std::string, GLenum> settings;
};

static inline auto ParseCameras(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Camera>> cameraVector;
    try {
        auto cameraIndex(0);
        for (const auto& camera : document["cameras"].GetArray()) {
            auto newCamera(Component::Create<Camera>("Camera" + std::to_string(cameraIndex)));
            if (std::string(camera["type"].GetString()) == "perspective") {
                auto perspective(camera["perspective"].GetObject());
                try {
                    newCamera->SetZfar(perspective["zfar"].GetFloat());
                } catch (std::exception&) {
                    debugLog("No zfar property")
                }
                try {
                    newCamera->SetZnear(perspective["znear"].GetFloat());
                } catch (std::exception&) {
                    debugLog("No znear property")
                }
                try {
                    newCamera->SetFov(glm::degrees(perspective["yfov"].GetFloat()));
                } catch (std::exception&) {
                    debugLog("No yfov property")
                }
            }
            cameraVector.push_back(newCamera);
            cameraIndex++;
        }
    } catch (std::exception&) {
        debugLog("No camera found")
    }
    return cameraVector;
}

static inline auto ParseTextureSamplers(const rapidjson::Document& document)
{
    std::vector<TextureSampler> samplerVector;
    try {
        for (const auto& sampler : document["samplers"].GetArray()) {
            TextureSampler newSampler;
            for (rapidjson::Value::ConstMemberIterator setting = sampler.MemberBegin(); setting != sampler.MemberEnd(); setting++) {
                newSampler.settings[setting->name.GetString()] = setting->value.GetInt();
            }
            samplerVector.push_back(newSampler);
        }
    } catch (std::exception&) {
        debugLog("No sampler found")
    }
    return samplerVector;
}

static inline auto ParseTextures(const std::filesystem::path path, const rapidjson::Document& document)
{
    debugLog("Start parsing textures");
    std::vector<std::shared_ptr<Texture2D>> textureVector;
    auto samplers = ParseTextureSamplers(document);
    try {
        auto textureIndex(0);
        for (const auto& textureValue : document["textures"].GetArray()) {
            std::string uri;
            try {
                auto source(textureValue["source"].GetInt());
                auto& image(document["images"].GetArray()[source]);
                uri = image["uri"].GetString();
                std::string header("data:application/octet-stream;base64,");
                if (uri.find(header) != 0) {
                    auto texturePath = std::filesystem::path(uri);
                    if (!texturePath.is_absolute())
                        texturePath = path.parent_path() / texturePath;
                    uri = texturePath.string();
                }
            } catch (std::exception&) {
                debugLog("Texture " + std::to_string(textureIndex) + " has no Uri")
            }
            std::shared_ptr<Texture2D> texture = nullptr;
            try {
                texture = TextureParser::parse("Texture " + std::to_string(textureIndex), uri);
                try {
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
                catch (std::exception&) {
                    debugLog("Texture " + std::to_string(textureIndex) + " has no sampler")
                }
            }
            catch (std::exception &e) {
                debugLog("Error parsing texture " + std::to_string(textureIndex) + ' ' + e.what());
            }
            textureVector.push_back(texture);
            textureIndex++;
        }
    } catch (std::exception&) {
        debugLog("No textures found")
    }
    debugLog("Done parsing textures");
    return textureVector;
}

#include "Material/MetallicRoughness.hpp"
#include "Material/SpecularGlossiness.hpp"

static inline auto ParseMaterialExtensions(const std::shared_ptr<AssetsContainer>& container, const rapidjson::Value& materialValue, std::shared_ptr<Material> material)
{
    try {
        for (const auto& extension : materialValue["extensions"].GetObject())
        {
            if (std::string(extension.name.GetString()) == "KHR_materials_pbrSpecularGlossiness")
            {
                const auto& pbrSpecularGlossiness = extension.value;
                auto materialExtension = Component::Create<SpecularGlossiness>();
                material->AddExtension(materialExtension);
                try {
                    auto diffuseFactor(pbrSpecularGlossiness["diffuseFactor"].GetArray());
                    materialExtension->SetDiffuse(glm::vec4(
                        diffuseFactor[0].GetFloat(),
                        diffuseFactor[1].GetFloat(),
                        diffuseFactor[2].GetFloat(),
                        diffuseFactor[3].GetFloat()
                    ));
                }
                catch (const std::exception&) {
                    debugLog("No diffuseFactor found for " + material->Name());
                }
                try {
                    auto diffuseFactor(pbrSpecularGlossiness["specularFactor"].GetArray());
                    materialExtension->SetSpecular(glm::vec3(
                        diffuseFactor[0].GetFloat(),
                        diffuseFactor[1].GetFloat(),
                        diffuseFactor[2].GetFloat()
                        ));
                }
                catch (const std::exception&) {
                    debugLog("No specularFactor found for " + material->Name());
                }
                try {
                    auto glossinessFactor(pbrSpecularGlossiness["glossinessFactor"].GetFloat());
                    materialExtension->SetGlossiness(glossinessFactor);
                }
                catch (const std::exception&) {
                    debugLog("No glossinessFactor found for " + material->Name());
                }
                try {
                    auto textureObject(pbrSpecularGlossiness["diffuseTexture"].GetObject());
                    materialExtension->SetTextureDiffuse(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
                }
                catch (std::exception&) {
                    debugLog("No diffuseTexture found for " + material->Name());
                }
                try {
                    auto textureObject(pbrSpecularGlossiness["specularGlossinessTexture"].GetObject());
                    materialExtension->SetTextureSpecularGlossiness(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
                }
                catch (std::exception&) {
                    debugLog("No specularGlossinessTexture found for " + material->Name());
                }
            }
        }
    }
    catch (std::exception&) {
        debugLog("No extension found");
    }
}

static inline auto ParseMaterials(const rapidjson::Document& document, const std::shared_ptr<AssetsContainer>& container)
{
    debugLog("Start parsing materials");
    //auto textureVector = ParseTextures(path, document);
    std::vector<std::shared_ptr<Material>> materialVector;
    try {
        auto materialIndex(0);
        for (const auto& materialValue : document["materials"].GetArray()) {
            auto material(Component::Create<Material>("Material " + std::to_string(materialIndex)));
            material->SetUVScale(glm::vec2(1, -1));
            try {
                material->SetName(materialValue["name"].GetString());
            }
            catch (std::exception&) {
                debugLog("Material " + material->Name() + " has no name property")
            }
            try {
                material->SetOpacityCutoff(materialValue["alphaCutoff"].GetFloat());
            }
            catch (std::exception&) {
                debugLog("Material " + material->Name() + " has no alphaCutoff property")
            }
            try {
                std::string alphaMode = materialValue["alphaMode"].GetString();
                if (alphaMode == "OPAQUE")
                    material->SetOpacityMode(Material::OpacityModeValue::Opaque);
                if (alphaMode == "MASK")
                    material->SetOpacityMode(Material::OpacityModeValue::Mask);
                if (alphaMode == "BLEND")
                    material->SetOpacityMode(Material::OpacityModeValue::Blend);
            }
            catch (std::exception&) {
                debugLog("Material " + material->Name() + " has no alphaCutoff property")
            }
            
            try {
                material->SetDoubleSided(materialValue["doubleSided"].GetBool());
            } catch (std::exception&) {
                debugLog("Material " + material->Name() + " has no doubleSided property")
            }
            try {
                auto emissiveFactor(materialValue["emissiveFactor"].GetArray());
                material->SetEmissive(glm::vec3(emissiveFactor[0].GetFloat(),
                    emissiveFactor[1].GetFloat(),
                    emissiveFactor[2].GetFloat()));
            } catch (std::exception&) {
                debugLog("No emissiveFactor property")
            }
            try {
                auto textureObject(materialValue["normalTexture"].GetObject());
                material->SetTextureNormal(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
            } catch (std::exception&) {
                debugLog("No normalTexture property")
            }
            try {
                auto textureObject(materialValue["emissiveTexture"].GetObject());
                material->SetTextureEmissive(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
            } catch (std::exception&) {
                debugLog("No emissiveTexture property")
            }
            try {
                auto textureObject(materialValue["occlusionTexture"].GetObject());
                material->SetTextureAO(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
            }
            catch (std::exception&) {
                debugLog("No occlusionTexture property")
            }
            try {
                auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
                auto materialExtension = Component::Create<MetallicRoughness>();
                material->AddExtension(materialExtension);
                materialExtension->SetRoughness(1);
                materialExtension->SetMetallic(1);
                try {
                    auto textureObject(pbrMetallicRoughness["metallicRoughnessTexture"].GetObject());
                    materialExtension->SetTextureMetallicRoughness(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No metallicRoughnessTexture property")
                }
                try {
                    auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
                    materialExtension->SetBaseColor(glm::vec4(baseColor[0].GetFloat(),
                        baseColor[1].GetFloat(),
                        baseColor[2].GetFloat(),
                        baseColor[3].GetFloat()));
                } catch (std::exception&) {
                    debugLog("No baseColorFactor property")
                }
                try {
                    materialExtension->SetMetallic(pbrMetallicRoughness["metallicFactor"].GetFloat());
                } catch (std::exception&) {
                    debugLog("No metallicFactor property")
                }
                try {
                    materialExtension->SetRoughness(pbrMetallicRoughness["roughnessFactor"].GetFloat());
                } catch (std::exception&) {
                    debugLog("No roughnessFactor property")
                }
                try {
                    auto textureObject(pbrMetallicRoughness["baseColorTexture"].GetObject());
                    materialExtension->SetTextureBaseColor(container->GetComponent<Texture2D>(textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No baseColorTexture property")
                }
            } catch (std::exception&) {
                debugLog("Not a pbrMetallicRoughness material")
            }
            ParseMaterialExtensions(container, materialValue, material);
            materialVector.push_back(material);
            materialIndex++;
        }
    } catch (std::exception& e) {
        debugLog("No materials found")
    }
    debugLog("Done parsing materials");
    return materialVector;
}

static inline auto ParseBuffers(const std::filesystem::path path, const rapidjson::Document& document)
{
    debugLog("Start parsing buffers");
    std::vector<std::shared_ptr<Buffer>> bufferVector;
    try {
        for (const auto& bufferValue : document["buffers"].GetArray()) {
            auto buffer(Component::Create<Buffer>(bufferValue["byteLength"].GetFloat()));
            /*try {
				std::string uri = bufferValue["uri"].GetString();
				std::string header("data:application/octet-stream;base64,");
				if (uri.find(header) == std::string::npos) {
					auto texturePath = std::filesystem::path(uri);
					if (!texturePath.is_absolute())
						texturePath = std::filesystem::path(path).parent_path()/texturePath;
					uri = texturePath.string();
				}
			}*/
            try {
                auto bufferPath(std::filesystem::path(bufferValue["uri"].GetString()));
                if (!bufferPath.is_absolute())
                    bufferPath = path.parent_path() / bufferPath;
                buffer->SetUri(bufferPath.string());
            } catch (std::exception&) {
                debugLog("Buffer " + buffer->Name() + " has no uri property")
            }
            try {
                buffer->SetName(bufferValue["name"].GetString());
            } catch (std::exception&) {
                debugLog("Buffer " + buffer->Name() + " has no name property")
            }
            bufferVector.push_back(buffer);
        }
    } catch (std::exception&) {
        debugLog("No buffers found")
    }
    debugLog("Done parsing buffers");
    return bufferVector;
}

static inline auto ParseBufferViews(const std::filesystem::path path, const rapidjson::Document& document)
{
    debugLog("Start parsing bufferViews");
    auto buffers(ParseBuffers(path, document));
    std::vector<std::shared_ptr<BufferView>> bufferViewVector;
    try {
        for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
            auto bufferView(Component::Create<BufferView>(
                bufferViewValue["byteLength"].GetInt(),
                buffers.at(bufferViewValue["buffer"].GetInt())));
            try {
                bufferView->SetByteOffset(bufferViewValue["byteOffset"].GetInt());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->Name() + " has no byteOffset property")
            }
            try {
                bufferView->SetByteStride(bufferViewValue["byteStride"].GetInt());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->Name() + " has no byteStride property")
            }
            try {
                bufferView->SetTarget(bufferViewValue["target"].GetInt());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->Name() + " has no target property")
            }
            try {
                bufferView->SetName(bufferViewValue["name"].GetString());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->Name() + " has no name property")
            }
            bufferViewVector.push_back(bufferView);
        }
    } catch (std::exception&) {
        debugLog("No bufferViews found")
    }
    debugLog("Done parsing bufferViews");
    return bufferViewVector;
}

static inline auto ParseBufferAccessors(const std::filesystem::path path, const rapidjson::Document& document)
{
    debugLog("Start parsing bufferAccessors");
    auto bufferViews(ParseBufferViews(path, document));
    std::vector<std::shared_ptr<BufferAccessor>> bufferAccessorVector;
    try {
        auto bufferAccessorIndex(0);
        for (const auto& bufferAccessorValue : document["accessors"].GetArray()) {
            auto bufferAccessor(Component::Create<BufferAccessor>(
                bufferAccessorValue["componentType"].GetInt(),
                bufferAccessorValue["count"].GetInt(),
                BufferAccessor::GetType(bufferAccessorValue["type"].GetString())));
            bufferAccessor->SetName("BufferAccessor " + std::to_string(bufferAccessorIndex));
            try {
                bufferAccessor->SetBufferView(bufferViews.at(bufferAccessorValue["bufferView"].GetInt()));
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->Name() + " has no bufferView property")
            }
            try {
                bufferAccessor->SetByteOffset(bufferAccessorValue["byteOffset"].GetInt());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->Name() + " has no byteOffset property")
            }
            try {
                bufferAccessor->SetNormalized(bufferAccessorValue["normalized"].GetBool());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->Name() + " has no normalized property")
            }
            try {
                bufferAccessor->SetCount(bufferAccessorValue["count"].GetInt());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->Name() + " has no count property")
            }
            try {
                bufferAccessor->SetName(bufferAccessorValue["name"].GetString());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->Name() + " has no name property")
            }
            bufferAccessorVector.push_back(bufferAccessor);
            bufferAccessorIndex++;
        }
    } catch (std::exception&) {
        debugLog("No bufferAccessors found")
    }
    debugLog("Done parsing bufferAccessors");
    return bufferAccessorVector;
}

static inline auto ParseMeshes(const rapidjson::Document& document, const std::shared_ptr<AssetsContainer>& container)
{
    debugLog("Start parsing meshes");
    std::vector<std::shared_ptr<Mesh>> meshVector;
    auto meshesItr(document.FindMember("meshes"));
    if (meshesItr == document.MemberEnd()) {
        debugLog("No meshes found");
        return meshVector;
    }
    auto defaultMaterial(Component::Create<Material>("defaultMaterial"));
    for (const auto& mesh : meshesItr->value.GetArray()) {
        debugLog("Found new mesh");
        auto currentMesh(Component::Create<Mesh>());
        try {
            currentMesh->SetName(mesh["name"].GetString());
        } catch (std::exception&) {
            debugLog("Mesh " + currentMesh->Name() + " has no name")
        }
        try {
            for (const auto& primitive : mesh["primitives"].GetArray()) {
                debugLog("Found new primitive");
                auto geometry(Component::Create<Geometry>());
                try {
                    auto& material(primitive["material"]);
                    if (size_t(material.GetInt()) >= container->GetComponentsNbr<Material>())
                        std::cerr << "Material index " << material.GetInt() << " out of bound " << container->GetComponentsNbr<Material>() << std::endl;
                    currentMesh->AddMaterial(container->GetComponent<Material>(material.GetInt()));
                    geometry->SetMaterialIndex(currentMesh->GetMaterialIndex(container->GetComponent<Material>(material.GetInt())));
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->Name() + " has no material")
                }
                try {
                    for (const auto& attribute : primitive["attributes"].GetObject()) {
                        auto attributeName(std::string(attribute.name.GetString()));
                        auto accessor(container->GetComponent<BufferAccessor>(attribute.value.GetInt()));
                        auto accessorKey(Geometry::GetAccessorKey(attributeName));
                        if (accessorKey == Geometry::AccessorKey::Invalid) {
                            debugLog("Invalid Accessor Key : " + attributeName);
                        } else
                            geometry->SetAccessor(accessorKey, accessor);
                    }
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->Name() + " has no material")
                }
                try {
                    auto accessor(container->GetComponent<BufferAccessor>(primitive["indices"].GetInt()));
                    geometry->SetIndices(accessor);
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->Name() + " has no indices property")
                }
                try {
                    geometry->SetMode(primitive["mode"].GetInt());
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->Name() + " has no mode property")
                }
                currentMesh->AddGeometry(geometry);
            }
        } catch (std::exception&) {
            debugLog("Mesh " + currentMesh->Name() + " has no primitives");
        }
        if (currentMesh->GetMaterial(0) == nullptr)
            currentMesh->AddMaterial(defaultMaterial);
        meshVector.push_back(currentMesh);
    }
    debugLog("Done parsing meshes");
    return meshVector;
}

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

static inline auto ParseNodes(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Node>> nodeVector;
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return nodeVector;
    int nodeIndex = 0;
    for (const auto& node : nodeItr->value.GetArray()) {
        auto newNode(Component::Create<Node>("Node_" + std::to_string(nodeIndex)));
        auto transform(newNode);
        try {
            newNode->SetName(node["name"].GetString());
        } catch (std::exception&) {
            debugLog("Node " + newNode->Name() + " has no name property");
        }
        try {
            glm::mat4 matrix;
            for (unsigned i(0); i < node["matrix"].GetArray().Size() && i < glm::uint(matrix.length() * 4); i++)
                matrix[i / 4][i % 4] = node["matrix"].GetArray()[i].GetFloat();
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, scale, rotation, translation, skew, perspective);
            transform->SetPosition(translation);
            transform->SetRotation(rotation);
            transform->SetScale(scale);
        } catch (std::exception&) {
            debugLog("Node " + newNode->Name() + " has no matrix property");
        }
        try {
            const auto& position(node["translation"].GetArray());
            glm::vec3 positionVec3;
            positionVec3[0] = position[0].GetFloat();
            positionVec3[1] = position[1].GetFloat();
            positionVec3[2] = position[2].GetFloat();
            transform->SetPosition(positionVec3);
        } catch (std::exception&) {
            debugLog("Node " + newNode->Name() + " has no translation property");
        }
        try {
            const auto& rotation(node["rotation"].GetArray());
            glm::quat rotationQuat;
            rotationQuat[0] = rotation[0].GetFloat();
            rotationQuat[1] = rotation[1].GetFloat();
            rotationQuat[2] = rotation[2].GetFloat();
            rotationQuat[3] = rotation[3].GetFloat();
            transform->SetRotation(glm::normalize(rotationQuat));
        } catch (std::exception&) {
            debugLog("Node " + newNode->Name() + " has no rotation property");
        }
        try {
            const auto& scale(node["scale"].GetArray());
            glm::vec3 scaleVec3;
            scaleVec3[0] = scale[0].GetFloat();
            scaleVec3[1] = scale[1].GetFloat();
            scaleVec3[2] = scale[2].GetFloat();
            transform->SetScale(scaleVec3);
        } catch (std::exception&) {
            debugLog("Node " + newNode->Name() + " has no scale property");
        }
        nodeVector.push_back(newNode);
        nodeIndex++;
    }
    return nodeVector;
}

static inline auto ParseAnimations(const rapidjson::Document& document, const std::shared_ptr<AssetsContainer>& container)
{
    std::vector<std::shared_ptr<Animation>> animations;
    try {
        for (const auto& animation : document["animations"].GetArray()) {
            auto newAnimation(Component::Create<Animation>());
            try {
                newAnimation->SetName(animation["name"].GetString());
            } catch (std::exception&) {
                debugLog("No name property found");
            }
            for (const auto& sampler : animation["samplers"].GetArray()) {
                auto samplerInput(container->GetComponent<BufferAccessor>(sampler["input"].GetInt()));
                auto samplerOutput(container->GetComponent<BufferAccessor>(sampler["output"].GetInt()));
                auto newSampler(AnimationSampler(samplerInput, samplerOutput));
                try {
                    std::string interpolation(sampler["interpolation"].GetString());
                    if (interpolation == "LINEAR")
                        newSampler.SetInterpolation(AnimationSampler::AnimationInterpolation::Linear);
                    else if (interpolation == "STEP")
                        newSampler.SetInterpolation(AnimationSampler::AnimationInterpolation::Step);
                    else if (interpolation == "CUBICSPLINE")
                        newSampler.SetInterpolation(AnimationSampler::AnimationInterpolation::CubicSpline);
                } catch (std::exception&) {
                    debugLog("No interpolation property found");
                }
                newAnimation->AddSampler(newSampler);
            }
            for (const auto& channel : animation["channels"].GetArray()) {
                AnimationChannel newChannel;
                try {
                    auto& target = (channel["target"]);
                    std::string path(target["path"].GetString());
                    newChannel.SetSamplerIndex(channel["sampler"].GetInt());
                    newChannel.SetTarget(container->GetComponent<Node>(target["node"].GetInt()));
                    if (path == "translation")
                        newChannel.SetPath(AnimationChannel::Channel::Translation);
                    else if (path == "rotation")
                        newChannel.SetPath(AnimationChannel::Channel::Rotation);
                    else if (path == "scale")
                        newChannel.SetPath(AnimationChannel::Channel::Scale);
                    else if (path == "weights")
                        newChannel.SetPath(AnimationChannel::Channel::Weights);
                } catch (std::exception&) {
                    debugLog("No target property found");
                }
                newAnimation->AddChannel(newChannel);
            }
            animations.push_back(newAnimation);
        }
    } catch (std::exception&) {
        debugLog("No animations found");
    }
    return animations;
}

static inline auto ParseSkins(const rapidjson::Document& document, const std::shared_ptr<AssetsContainer>& container)
{
    debugLog("Start parsing Skins");
    std::vector<std::shared_ptr<MeshSkin>> skins;
    try {
        for (const auto& skin : document["skins"].GetArray()) {
            auto newSkin(Component::Create<MeshSkin>());
            try {
                newSkin->SetName(skin["name"].GetString());
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no name");
            }
            try {
                newSkin->SetInverseBinMatrices(container->GetComponent<BufferAccessor>(skin["inverseBindMatrices"].GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no inverseBindMatrices");
            }
            /*try {
                newSkin->SetSkeleton(container->GetComponents<Node>().at(skin["skeleton"].GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no skeleton");
            }*/
            try {
                for (const auto& joint : skin["joints"].GetArray())
                    newSkin->AddJoint(container->GetComponent<Node>(joint.GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no joints");
            }
            skins.push_back(newSkin);
        }
    } catch (std::exception&) {
        debugLog("No skins found");
    }
    debugLog("Done parsing Skins");
    return skins;
}

static inline auto SetParenting(const rapidjson::Document& document, const std::shared_ptr<AssetsContainer>& container)
{
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return;
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : nodeItr->value.GetArray()) {
        auto node(container->GetComponent<Node>(nodeIndex));
        try {
            auto mesh(container->GetComponent<Mesh>(gltfNode["mesh"].GetInt()));
            node->SetComponent(mesh);
            try {
                mesh->SetComponent(container->GetComponent<MeshSkin>(gltfNode["skin"].GetInt()));
            } catch (std::exception&) {
                debugLog("Mesh " + mesh->Name() + " has no skin");
            }
        } catch (std::exception&) {
            debugLog("Node " + node->Name() + " has no mesh");
        }
        try {
            auto camera = container->GetComponent<Camera>(gltfNode["camera"].GetInt());
            //camera->SetParent(node);
            node->AddChild(camera);
            //container->GetComponents<Camera>().at(gltfNode["camera"].GetInt())->SetParent(node);
        } catch (std::exception&) {
            debugLog("Node " + node->Name() + " has no camera");
        }
        try {
            for (const auto& child : gltfNode["children"].GetArray()) {
                auto childNode = container->GetComponent<Node>(child.GetInt());
                //childNode->SetParent(node);
                node->AddChild(container->GetComponent<Node>(child.GetInt()));
                //container->GetComponents<Node>().at(child.GetInt())->SetParent(node);
                std::cout << "Node parenting " << node->Name() << " -> " << container->GetComponent<Node>(child.GetInt())->Name() << std::endl;
            }
        } catch (std::exception&) {
            debugLog("Node " + node->Name() + " has no skeleton");
        }
        nodeIndex++;
    }
}

static inline auto ParseScenes(const rapidjson::Document& document, const std::shared_ptr<AssetsContainer>& container)
{
    std::vector<std::shared_ptr<Scene>> sceneVector;
    auto scenes(document["scenes"].GetArray());
    int sceneIndex = 0;
    for (const auto& scene : scenes) {
        std::cout << "found scene" << std::endl;
        auto newScene(Component::Create<Scene>(std::to_string(sceneIndex)));
        for (const auto& node : scene["nodes"].GetArray()) {
            newScene->AddRootNode(container->GetComponent<Node>(node.GetInt()));
            std::cout << container->GetComponent<Node>(node.GetInt())->Name() << std::endl;
        }
        for (const auto& animation : container->GetComponents<Animation>()) {
            newScene->Add(animation);
            std::cout << (animation ? animation->Name() : "nullptr") << std::endl;
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

#include "Light/DirectionnalLight.hpp"

static inline auto ParseLights(const rapidjson::Document& document) {
    std::vector<std::shared_ptr<Light>> lightsVector;
    const auto& extensions(document.FindMember("extensions"));
    if (extensions != document.MemberEnd())
    {
        const auto& KHR_lights_punctual = extensions->value.FindMember("KHR_lights_punctual");
        if (KHR_lights_punctual != extensions->value.MemberEnd()) {
            const auto& lights = KHR_lights_punctual->value.FindMember("lights");
            if (lights != KHR_lights_punctual->value.MemberEnd()) {
                for (const auto& light : lights->value.GetArray()) {
                    std::shared_ptr<Light> newLight;
                    try {
                        auto type = light["type"].GetString();
                        if (type == "directional")
                            newLight = Component::Create<DirectionnalLight>();
                        lightsVector.emplace_back(newLight);
                    }
                    catch (std::exception&) {
                        debugLog("[MISSING REQUIRED VALUE] : Light has no type");
                    }
                }
            }
        }
    }
    return lightsVector;
}

std::shared_ptr<AssetsContainer> GLTF::Parse(const std::filesystem::path path)
{
    std::shared_ptr<AssetsContainer> container = Component::Create<AssetsContainer>();
    std::cout << path << std::endl;
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file !");
        return container;
    }
    for (auto& node : ParseNodes(document)) {
        container->AddComponent(node);
    }
    for (auto& camera : ParseCameras(document)) {
        container->AddComponent(camera);
    }
    for (auto& accessor : ParseBufferAccessors(path, document)) {
        container->AddComponent(accessor);
    }
    for (auto& texture : ParseTextures(path, document)) {
        container->AddComponent(texture);
    }
    for (auto& material : ParseMaterials(document, container)) {
        container->AddComponent(material);
    }
    for (auto& mesh : ParseMeshes(document, container)) {
        container->AddComponent(mesh);
    }
    for (auto& skin : ParseSkins(document, container)) {
        container->AddComponent(skin);
    }
    for (auto& animation : ParseAnimations(document, container)) {
        container->AddComponent(animation);
    }
    SetParenting(document, container);
    for (auto& scene : ParseScenes(document, container)) {
        container->AddComponent(scene);
    }
    return container;
}