/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 17:15:22
*/

#include "Parser/GLTF.hpp"
#include "Animation/Animation.hpp"
#include "Animation/AnimationSampler.hpp"
#include "Assets/AssetsContainer.hpp"
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
            auto newCamera(Camera::Create("Camera" + std::to_string(cameraIndex), 45));
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
            std::shared_ptr<Texture2D> texture = nullptr;//Texture2D::Create("Texture_" + std::to_string(textureIndex), glm::ivec2(0), GL_TEXTURE_2D, GL_RGB, GL_RGB);
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

static inline std::shared_ptr<Texture2D> GetTexture(const std::vector<std::shared_ptr<Texture2D>> textureVector, size_t index)
{
    try {
        return textureVector.at(index);
    } catch (std::exception& e) {
        std::cerr << "Error getting texture : " << e.what() << std::endl;
    }
    return nullptr;
}

static inline auto ParseMaterials(const std::filesystem::path path, const rapidjson::Document& document)
{
    debugLog("Start parsing materials");
    auto textureVector = ParseTextures(path, document);
    std::vector<std::shared_ptr<Material>> materialVector;
    try {
        auto materialIndex(0);
        for (const auto& materialValue : document["materials"].GetArray()) {
            auto material(Material::Create("Material " + std::to_string(materialIndex)));
            material->SetUVScale(glm::vec2(1, -1));
            material->SetAlbedo(glm::vec3(1));
            try {
                material->SetDoubleSided(materialValue["doubleSided"].GetBool());
            } catch (std::exception&) {
                debugLog("Material " + material->Name() + " has no doubleSided property")
            }
            try {
                auto emissiveFactor(materialValue["emissiveFactor"].GetArray());
                material->SetEmitting(glm::vec3(emissiveFactor[0].GetFloat(),
                    emissiveFactor[1].GetFloat(),
                    emissiveFactor[2].GetFloat()));
            } catch (std::exception&) {
                debugLog("No emissiveFactor property")
            }
            try {
                auto textureObject(materialValue["normalTexture"].GetObject());
                material->SetTextureNormal(GetTexture(textureVector, textureObject["index"].GetInt()));
            } catch (std::exception&) {
                debugLog("No normalTexture property")
            }
            try {
                auto textureObject(materialValue["emissiveTexture"].GetObject());
                material->SetTextureEmitting(GetTexture(textureVector, textureObject["index"].GetInt()));
            } catch (std::exception&) {
                debugLog("No emissiveTexture property")
            }
            try {
                auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
                try {
                    auto textureObject(pbrMetallicRoughness["metallicRoughnessTexture"].GetObject());
                    material->SetTextureMetallicRoughness(GetTexture(textureVector, textureObject["index"].GetInt()));
                    material->SetRoughness(1);
                    material->SetMetallic(1);
                } catch (std::exception&) {
                    debugLog("No metallicRoughnessTexture property")
                }
                try {
                    auto textureObject(pbrMetallicRoughness["occlusionTexture"].GetObject());
                    material->SetTextureAO(GetTexture(textureVector, textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No occlusionTexture property")
                }
                try {
                    auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
                    material->SetAlbedo(glm::vec3(baseColor[0].GetFloat(),
                        baseColor[1].GetFloat(),
                        baseColor[2].GetFloat()));
                    material->SetAlpha(baseColor[3].GetFloat());
                } catch (std::exception&) {
                    debugLog("No baseColorFactor property")
                }
                try {
                    material->SetMetallic(pbrMetallicRoughness["metallicFactor"].GetFloat());
                } catch (std::exception&) {
                    debugLog("No metallicFactor property")
                }
                try {
                    material->SetRoughness(pbrMetallicRoughness["roughnessFactor"].GetFloat());
                } catch (std::exception&) {
                    debugLog("No roughnessFactor property")
                }
                try {
                    auto textureObject(pbrMetallicRoughness["baseColorTexture"].GetObject());
                    material->SetTextureAlbedo(GetTexture(textureVector, textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No baseColorTexture property")
                }
            } catch (std::exception&) {
                debugLog("Not a pbrMetallicRoughness material")
            }
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
        auto bufferIndex(0);
        for (const auto& bufferValue : document["buffers"].GetArray()) {
            auto buffer(Buffer::Create(bufferValue["byteLength"].GetFloat()));
            buffer->SetName("Buffer " + std::to_string(bufferIndex));
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
            bufferIndex++;
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
        auto bufferViewIndex(0);
        for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
            auto bufferView(BufferView::Create(
                bufferViewValue["byteLength"].GetInt(),
                buffers.at(bufferViewValue["buffer"].GetInt())));
            bufferView->SetName("BufferView " + std::to_string(bufferViewIndex));
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
            bufferViewIndex++;
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
            auto bufferAccessor(BufferAccessor::Create(
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

static inline auto ParseMeshes(const rapidjson::Document& document, const AssetsContainer& container)
{
    debugLog("Start parsing meshes");
    std::vector<std::shared_ptr<Mesh>> meshVector;
    auto meshesItr(document.FindMember("meshes"));
    if (meshesItr == document.MemberEnd()) {
        debugLog("No meshes found");
        return meshVector;
    }
    auto defaultMaterial(Material::Create("defaultMaterial"));
    for (const auto& mesh : meshesItr->value.GetArray()) {
        debugLog("Found new mesh");
        auto currentMesh(Mesh::Create());
        try {
            currentMesh->SetName(mesh["name"].GetString());
        } catch (std::exception&) {
            debugLog("Mesh " + currentMesh->Name() + " has no name")
        }
        try {
            for (const auto& primitive : mesh["primitives"].GetArray()) {
                debugLog("Found new primitive");
                auto geometry(Geometry::Create());
                try {
                    auto& material(primitive["material"]);
                    auto materials = container.GetComponents<Material>();
                    if (size_t(material.GetInt()) >= container.GetComponents<Material>().size())
                        std::cerr << "Material index " << material.GetInt() << " out of bound " << container.GetComponents<Material>().size() << std::endl;
                    currentMesh->AddMaterial(materials.at(material.GetInt()));
                    geometry->SetMaterialIndex(currentMesh->GetMaterialIndex(materials.at(material.GetInt())));
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->Name() + " has no material")
                }
                try {
                    for (const auto& attribute : primitive["attributes"].GetObject()) {
                        auto attributeName(std::string(attribute.name.GetString()));
                        auto accessor(container.GetComponents<BufferAccessor>().at(attribute.value.GetInt()));
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
                    auto accessor(container.GetComponents<BufferAccessor>().at(primitive["indices"].GetInt()));
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
        auto newNode(Node::Create("Node_" + std::to_string(nodeIndex)));
        auto transform(newNode->GetComponent<Transform>());
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

static inline auto ParseAnimations(const rapidjson::Document& document, const AssetsContainer& container)
{
    std::vector<std::shared_ptr<Animation>> animations;
    try {
        for (const auto& animation : document["animations"].GetArray()) {
            auto newAnimation(Animation::Create());
            try {
                newAnimation->SetName(animation["name"].GetString());
            } catch (std::exception&) {
                debugLog("No name property found");
            }
            for (const auto& sampler : animation["samplers"].GetArray()) {
                auto samplerInput(container.GetComponents<BufferAccessor>().at(sampler["input"].GetInt()));
                auto samplerOutput(container.GetComponents<BufferAccessor>().at(sampler["output"].GetInt()));
                auto newSampler(AnimationSampler(samplerInput, samplerOutput));
                try {
                    std::string interpolation(sampler["interpolation"].GetString());
                    if (interpolation == "LINEAR")
                        newSampler.SetInterpolation(AnimationSampler::Linear);
                    else if (interpolation == "STEP")
                        newSampler.SetInterpolation(AnimationSampler::Step);
                    else if (interpolation == "CUBICSPLINE")
                        newSampler.SetInterpolation(AnimationSampler::CubicSpline);
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
                    newChannel.SetTarget(container.GetComponents<Node>().at(target["node"].GetInt()));
                    if (path == "translation")
                        newChannel.SetPath(AnimationChannel::Translation);
                    else if (path == "rotation")
                        newChannel.SetPath(AnimationChannel::Rotation);
                    else if (path == "scale")
                        newChannel.SetPath(AnimationChannel::Scale);
                    else if (path == "weights")
                        newChannel.SetPath(AnimationChannel::Weights);
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

static inline auto ParseSkins(const rapidjson::Document& document, const AssetsContainer& container)
{
    debugLog("Start parsing Skins");
    std::vector<std::shared_ptr<MeshSkin>> skins;
    try {
        auto skinIndex(0u);
        for (const auto& skin : document["skins"].GetArray()) {
            auto newSkin(MeshSkin::Create());
            newSkin->SetName("Skin_" + std::to_string(skinIndex));
            try {
                newSkin->SetName(skin["name"].GetString());
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no name");
            }
            try {
                newSkin->SetInverseBinMatrices(container.GetComponents<BufferAccessor>().at(skin["inverseBindMatrices"].GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no inverseBindMatrices");
            }
            try {
                newSkin->SetSkeleton(container.GetComponents<Node>().at(skin["skeleton"].GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->Name() + " has no skeleton");
            }
            try {
                for (const auto& joint : skin["joints"].GetArray())
                    newSkin->AddJoint(container.GetComponents<Node>().at(joint.GetInt()));
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

static inline auto SetParenting(const rapidjson::Document& document, const AssetsContainer& container)
{
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return;
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : nodeItr->value.GetArray()) {
        auto node(container.GetComponents<Node>().at(nodeIndex));
        try {
            auto mesh(container.GetComponents<Mesh>().at(gltfNode["mesh"].GetInt()));
            node->SetComponent(mesh);
            try {
                mesh->SetComponent(container.GetComponents<MeshSkin>().at(gltfNode["skin"].GetInt()));
            } catch (std::exception&) {
                debugLog("Mesh " + mesh->Name() + " has no skin");
            }
        } catch (std::exception&) {
            debugLog("Node " + node->Name() + " has no mesh");
        }
        try {
            container.GetComponents<Camera>().at(gltfNode["camera"].GetInt())->SetParent(node);
        } catch (std::exception&) {
            debugLog("Node " + node->Name() + " has no camera");
        }
        try {
            for (const auto& child : gltfNode["children"].GetArray()) {
                container.GetComponents<Node>().at(child.GetInt())->SetParent(node);
                std::cout << "Node parenting " << node->Name() << " -> " << container.GetComponents<Node>().at(child.GetInt())->Name() << std::endl;
            }
        } catch (std::exception&) {
            debugLog("Node " + node->Name() + " has no skeleton");
        }
        nodeIndex++;
    }
}

static inline auto ParseScenes(const rapidjson::Document& document, const AssetsContainer& container)
{
    std::vector<std::shared_ptr<Scene>> sceneVector;
    auto scenes(document["scenes"].GetArray());
    int sceneIndex = 0;
    for (const auto& scene : scenes) {
        std::cout << "found scene" << std::endl;
        auto newScene(Scene::Create(std::to_string(sceneIndex)));
        for (const auto& node : scene["nodes"].GetArray()) {
            newScene->AddRootNode(container.GetComponents<Node>().at(node.GetInt()));
            std::cout << container.GetComponents<Node>().at(node.GetInt())->Name() << std::endl;
        }
        for (const auto& animation : container.GetComponents<Animation>()) {
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

AssetsContainer GLTF::Parse(const std::filesystem::path path)
{
    AssetsContainer container;
    std::cout << path << std::endl;
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file !");
        return container;
    }
    for (const auto& node : ParseNodes(document)) {
        container.AddComponent(node);
    }
    for (const auto& camera : ParseCameras(document)) {
        container.AddComponent(camera);
    }
    for (const auto& accessor : ParseBufferAccessors(path, document)) {
        container.AddComponent(accessor);
    }
    for (const auto& material : ParseMaterials(path, document)) {
        container.AddComponent(material);
    }
    for (const auto& mesh : ParseMeshes(document, container)) {
        container.AddComponent(mesh);
    }
    for (const auto& skin : ParseSkins(document, container)) {
        container.AddComponent(skin);
    }
    for (const auto& animation : ParseAnimations(document, container)) {
        container.AddComponent(animation);
    }
    SetParenting(document, container);
    for (const auto& scene : ParseScenes(document, container)) {
        container.AddComponent(scene);
    }
    return container;
}