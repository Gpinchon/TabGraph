/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-11 16:17:13
*/

#include "Animation/Animation.hpp"
#include "Animation/AnimationSampler.hpp"
#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"
#include "Assets/BinaryData.hpp"
#include "Assets/Image.hpp"
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

#include <filesystem>
#include <fstream>
#include <glm/ext.hpp>
#include <iostream>
#include <memory>

void ParseGLTF(std::shared_ptr<Asset>);

auto GLTFMimeExtension {
    AssetsParser::AddMimeExtension("model/gltf+json", ".gltf") //not standard but screw it.
};

auto GLTFMimesParsers {
    AssetsParser::Add("model/gltf+json", ParseGLTF)
};

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

Uri CreateUri(const std::filesystem::path& parentPath, const std::string& dataPath)
{
    auto bufferPath { std::filesystem::path(dataPath) };
    if (bufferPath.string().rfind("data:", 0) == 0)
        return Uri(bufferPath.string());
    else {
        if (!bufferPath.is_absolute())
            bufferPath = parentPath / bufferPath;
        return Uri(bufferPath);
    }
}

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

static inline auto ParseTextures(const rapidjson::Document& document, std::vector<std::shared_ptr<Asset>>& images)
{
    debugLog("Start parsing textures");
    std::vector<std::shared_ptr<Texture2D>> textureVector;
    auto samplers = ParseTextureSamplers(document);
    try {
        auto textureIndex(0);
        for (const auto& textureValue : document["textures"].GetArray()) {
            std::shared_ptr<Texture2D> texture = nullptr;
            try {
                auto source(textureValue["source"].GetInt());
                auto image { images.at(source) };
                texture = Component::Create<Texture2D>(image);
            } catch (std::exception&) {
                debugLog("Texture " + std::to_string(textureIndex) + " has no source");
                texture = Component::Create<Texture2D>(glm::ivec2(1), Pixel::SizedFormat::Uint8_NormalizedR);
            }
            try {
                try {
                    auto sampler(samplers.at(textureValue["sampler"].GetInt()));
                    if (sampler.settings["magFilter"] != 0)
                        texture->SetParameter<Texture::Parameter::MagFilter>((Texture::Filter)sampler.settings["magFilter"]);
                    if (sampler.settings["minFilter"] != 0)
                        texture->SetParameter<Texture::Parameter::MinFilter>((Texture::Filter)sampler.settings["magFilter"]);
                    if (sampler.settings["wrapS"] != 0)
                        texture->SetParameter<Texture::Parameter::WrapS>((Texture::Wrap)sampler.settings["wrapS"]);
                    if (sampler.settings["wrapT"] != 0)
                        texture->SetParameter<Texture::Parameter::WrapT>((Texture::Wrap)sampler.settings["wrapT"]);
                } catch (std::exception&) {
                    debugLog("Texture " + std::to_string(textureIndex) + " has no sampler")
                }
            } catch (std::exception& e) {
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

static inline auto ParseMaterialExtensions(const std::vector<std::shared_ptr<Texture2D>>& textures, const rapidjson::Value& materialValue, std::shared_ptr<Material> material)
{
    try {
        for (const auto& extension : materialValue["extensions"].GetObject()) {
            if (std::string(extension.name.GetString()) == "KHR_materials_pbrSpecularGlossiness") {
                const auto& pbrSpecularGlossiness = extension.value;
                auto materialExtension = Component::Create<SpecularGlossiness>();
                material->AddExtension(materialExtension);
                try {
                    auto diffuseFactor(pbrSpecularGlossiness["diffuseFactor"].GetArray());
                    materialExtension->SetDiffuse(glm::vec3(
                        diffuseFactor[0].GetFloat(),
                        diffuseFactor[1].GetFloat(),
                        diffuseFactor[2].GetFloat()));
                    materialExtension->SetOpacity(diffuseFactor[3].GetFloat());
                } catch (const std::exception&) {
                    debugLog("No diffuseFactor found for " + material->GetName());
                }
                try {
                    auto diffuseFactor(pbrSpecularGlossiness["specularFactor"].GetArray());
                    materialExtension->SetSpecular(glm::vec3(
                        diffuseFactor[0].GetFloat(),
                        diffuseFactor[1].GetFloat(),
                        diffuseFactor[2].GetFloat()));
                } catch (const std::exception&) {
                    debugLog("No specularFactor found for " + material->GetName());
                }
                try {
                    auto glossinessFactor(pbrSpecularGlossiness["glossinessFactor"].GetFloat());
                    materialExtension->SetGlossiness(glossinessFactor);
                } catch (const std::exception&) {
                    debugLog("No glossinessFactor found for " + material->GetName());
                }
                try {
                    auto textureObject(pbrSpecularGlossiness["diffuseTexture"].GetObject());
                    materialExtension->SetTextureDiffuse(textures.at(textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No diffuseTexture found for " + material->GetName());
                }
                try {
                    auto textureObject(pbrSpecularGlossiness["specularGlossinessTexture"].GetObject());
                    materialExtension->SetTextureSpecularGlossiness(textures.at(textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No specularGlossinessTexture found for " + material->GetName());
                }
            }
        }
    } catch (std::exception&) {
        debugLog("No extension found");
    }
}

static inline auto ParseMaterials(const rapidjson::Document& document, std::vector<std::shared_ptr<Texture2D>>& textures)
{
    debugLog("Start parsing materials");
    //auto textureVector = ParseTextures(path, document);
    std::vector<std::shared_ptr<Material>> materialVector;
    try {
        auto materialIndex(0);
        for (const auto& materialValue : document["materials"].GetArray()) {
            auto material(Component::Create<Material>("Material " + std::to_string(materialIndex)));
            material->SetUVScale(glm::vec2(1, 1));
            try {
                material->SetName(materialValue["name"].GetString());
            } catch (std::exception&) {
                debugLog("Material " + material->GetName() + " has no name property")
            }
            try {
                material->SetOpacityCutoff(materialValue["alphaCutoff"].GetFloat());
            } catch (std::exception&) {
                debugLog("Material " + material->GetName() + " has no alphaCutoff property")
            }
            try {
                std::string alphaMode = materialValue["alphaMode"].GetString();
                if (alphaMode == "Opaque")
                    material->SetOpacityMode(Material::OpacityMode::Opaque);
                if (alphaMode == "MASK")
                    material->SetOpacityMode(Material::OpacityMode::Mask);
                if (alphaMode == "BLEND")
                    material->SetOpacityMode(Material::OpacityMode::Blend);
            } catch (std::exception&) {
                debugLog("Material " + material->GetName() + " has no alphaCutoff property")
            }

            try {
                material->SetDoubleSided(materialValue["doubleSided"].GetBool());
            } catch (std::exception&) {
                debugLog("Material " + material->GetName() + " has no doubleSided property")
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
                material->SetTextureNormal(textures.at(textureObject["index"].GetInt()));
            } catch (std::exception&) {
                debugLog("No normalTexture property")
            }
            try {
                auto textureObject(materialValue["emissiveTexture"].GetObject());
                material->SetTextureEmissive(textures.at(textureObject["index"].GetInt()));
            } catch (std::exception&) {
                debugLog("No emissiveTexture property")
            }
            try {
                auto textureObject(materialValue["occlusionTexture"].GetObject());
                material->SetTextureAO(textures.at(textureObject["index"].GetInt()));
            } catch (std::exception&) {
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
                    materialExtension->SetTextureMetallicRoughness(textures.at(textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No metallicRoughnessTexture property")
                }
                try {
                    auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
                    materialExtension->SetBaseColor(glm::vec3(baseColor[0].GetFloat(),
                        baseColor[1].GetFloat(),
                        baseColor[2].GetFloat()));
                    materialExtension->SetOpacity(baseColor[3].GetFloat());
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
                    materialExtension->SetTextureBaseColor(textures.at(textureObject["index"].GetInt()));
                } catch (std::exception&) {
                    debugLog("No baseColorTexture property")
                }
            } catch (std::exception&) {
                debugLog("Not a pbrMetallicRoughness material")
            }
            ParseMaterialExtensions(textures, materialValue, material);
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
    std::vector<std::shared_ptr<Asset>> bufferVector;
    //try {
    for (const auto& bufferValue : document["buffers"].GetArray()) {
        //auto buffer(Component::Create<BinaryData>(bufferValue["byteLength"].GetFloat()));
        auto asset { Component::Create<Asset>() };
        //try {
        auto uri = CreateUri(path.parent_path(), bufferValue["uri"].GetString());
        asset->SetUri(uri);
        /*auto bufferPath(std::filesystem::path(bufferValue["uri"].GetString()));
                if (bufferPath.root_name() == "data")
                    asset->SetUri(bufferPath.string());
                else {
                    if (!bufferPath.is_absolute())
                        bufferPath = path.parent_path() / bufferPath;
                    asset->SetUri(bufferPath);
                }*/
        //} catch (std::exception&) {
        //    debugLog("BinaryData " + asset->GetName() + " has no uri property")
        //}
        try {
            asset->SetName(bufferValue["name"].GetString());
        } catch (std::exception&) {
            debugLog("BinaryData " + asset->GetName() + " has no name property")
        }
        bufferVector.push_back(asset);
    }
    //} catch (std::exception&) {
    //    debugLog("No buffers found")
    // }
    debugLog("Done parsing buffers");
    return bufferVector;
}

static inline BufferView::Type GetBufferViewType(GLenum type)
{
    switch (type) {
    case GL_ARRAY_BUFFER:
        return BufferView::Type::Array;
    case GL_ELEMENT_ARRAY_BUFFER:
        return BufferView::Type::ElementArray;
    default:
        return BufferView::Type::Unknown;
    }
}

static inline auto ParseBufferViews(const rapidjson::Document& document, std::vector<std::shared_ptr<Asset>> buffers)
{
    debugLog("Start parsing bufferViews");
    std::vector<std::shared_ptr<BufferView>> bufferViewVector;
    try {
        for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
            auto bufferView(Component::Create<BufferView>(
                bufferViewValue["byteLength"].GetInt(),
                buffers.at(bufferViewValue["buffer"].GetInt())));
            try {
                bufferView->SetByteOffset(bufferViewValue["byteOffset"].GetInt());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->GetName() + " has no byteOffset property")
            }
            try {
                bufferView->SetByteStride(bufferViewValue["byteStride"].GetInt());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->GetName() + " has no byteStride property")
            }
            try {
                bufferView->SetType(GetBufferViewType(bufferViewValue["target"].GetInt()));
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->GetName() + " has no target property")
            }
            try {
                bufferView->SetName(bufferViewValue["name"].GetString());
            } catch (std::exception&) {
                debugLog("BufferView " + bufferView->GetName() + " has no name property")
            }
            bufferViewVector.push_back(bufferView);
        }
    } catch (std::exception&) {
        debugLog("No bufferViews found")
    }
    debugLog("Done parsing bufferViews");
    return bufferViewVector;
}

static inline BufferAccessor::ComponentType GetBufferAccessorComponentType(GLenum type)
{
    switch (type) {
    case 5120: //GL_BYTE:
        return BufferAccessor::ComponentType::Int8;
    case 5121: //GL_UNSIGNED_BYTE:
        return BufferAccessor::ComponentType::Uint8;
    case 5122: //GL_SHORT:
        return BufferAccessor::ComponentType::Int16;
    case 5123: //GL_UNSIGNED_SHORT:
        return BufferAccessor::ComponentType::Uint16;
    case 5125: //GL_UNSIGNED_INT:
        return BufferAccessor::ComponentType::Uint32;
    case 5126: //GL_FLOAT:
        return BufferAccessor::ComponentType::Float32;
    default:
        return BufferAccessor::ComponentType::Unknown;
    }
}

static inline BufferAccessor::Type GetBufferAccessorType(const std::string& type)
{
    if (type == "SCALAR")
        return BufferAccessor::Type::Scalar;
    else if (type == "VEC2")
        return BufferAccessor::Type::Vec2;
    else if (type == "VEC3")
        return BufferAccessor::Type::Vec3;
    else if (type == "VEC4")
        return BufferAccessor::Type::Vec4;
    else if (type == "MAT2")
        return BufferAccessor::Type::Mat2;
    else if (type == "MAT3")
        return BufferAccessor::Type::Mat3;
    else if (type == "MAT4")
        return BufferAccessor::Type::Mat4;
    return BufferAccessor::Type::Unknown;
}

static inline auto ParseBufferAccessors(const rapidjson::Document& document, std::vector<std::shared_ptr<BufferView>>& bufferViews)
{
    debugLog("Start parsing bufferAccessors");
    std::vector<std::shared_ptr<BufferAccessor>> bufferAccessorVector;
    try {
        auto bufferAccessorIndex(0);
        for (const auto& bufferAccessorValue : document["accessors"].GetArray()) {
            auto bufferAccessor(Component::Create<BufferAccessor>(
                GetBufferAccessorComponentType(bufferAccessorValue["componentType"].GetInt()),
                GetBufferAccessorType(bufferAccessorValue["type"].GetString()),
                bufferAccessorValue["count"].GetInt()));
            bufferAccessor->SetName("BufferAccessor " + std::to_string(bufferAccessorIndex));
            try {
                bufferAccessor->SetBufferView(bufferViews.at(bufferAccessorValue["bufferView"].GetInt()));
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->GetName() + " has no bufferView property")
            }
            try {
                bufferAccessor->SetByteOffset(bufferAccessorValue["byteOffset"].GetInt());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->GetName() + " has no byteOffset property")
            }
            try {
                bufferAccessor->SetNormalized(bufferAccessorValue["normalized"].GetBool());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->GetName() + " has no normalized property")
            }
            /*try {
                bufferAccessor->SetCount(bufferAccessorValue["count"].GetInt());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->GetName() + " has no count property")
            }*/
            try {
                bufferAccessor->SetName(bufferAccessorValue["name"].GetString());
            } catch (std::exception&) {
                debugLog("Accessor " + bufferAccessor->GetName() + " has no name property")
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

static inline Geometry::DrawingMode GetGeometryDrawingMode(GLenum mode)
{
    switch (mode) {
    case 0: //GL_POINTS:
        return Geometry::DrawingMode::Points;
    case 1: //GL_LINES:
        return Geometry::DrawingMode::Lines;
    case 2: //GL_LINE_LOOP:
        return Geometry::DrawingMode::LineLoop;
    case 3: //GL_LINE_STRIP:
        return Geometry::DrawingMode::LineStrip;
    case 4: //GL_TRIANGLES:
        return Geometry::DrawingMode::Triangles;
    case 5: //GL_TRIANGLE_STRIP:
        return Geometry::DrawingMode::TriangleStrip;
    case 6: //GL_TRIANGLE_FAN:
        return Geometry::DrawingMode::TriangleFan;
    default:
        return Geometry::DrawingMode::Unknown;
    }
}

static inline auto ParseMeshes(const rapidjson::Document& document, const std::vector<std::shared_ptr<Material>>& materials, const std::vector<std::shared_ptr<BufferAccessor>>& bufferAccessors)
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
            debugLog("Mesh " + currentMesh->GetName() + " has no name")
        }
        try {
            for (const auto& primitive : mesh["primitives"].GetArray()) {
                debugLog("Found new primitive");
                auto geometry(Component::Create<Geometry>());
                auto materialPtr{ defaultMaterial };
                try {
                    auto& material(primitive["material"]);
                    if (size_t(material.GetInt()) >= materials.size())
                        std::cerr << "Material index " << material.GetInt() << " out of bound " << materials.size() << std::endl;
                    materialPtr = materials.at(material.GetInt());
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->GetName() + " has no material")
                }
                try {
                    for (const auto& attribute : primitive["attributes"].GetObject()) {
                        auto attributeName(std::string(attribute.name.GetString()));
                        auto accessor(bufferAccessors.at(attribute.value.GetInt()));
                        auto accessorKey(Geometry::GetAccessorKey(attributeName));
                        if (accessorKey == Geometry::AccessorKey::Invalid) {
                            debugLog("Invalid Accessor Key : " + attributeName);
                        } else {
                            geometry->SetAccessor(accessorKey, accessor);
                            accessor->GetBufferView()->SetType(BufferView::Type::Array);
                        }
                    }
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->GetName() + " has no attributes")
                }
                try {
                    auto accessor(bufferAccessors.at(primitive["indices"].GetInt()));
                    geometry->SetIndices(accessor);
                    accessor->GetBufferView()->SetType(BufferView::Type::ElementArray);
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->GetName() + " has no indices property")
                }
                try {
                    geometry->SetDrawingMode(GetGeometryDrawingMode(primitive["mode"].GetInt()));
                } catch (std::exception&) {
                    debugLog("Geometry " + geometry->GetName() + " has no mode property")
                }
                currentMesh->AddGeometry(geometry, materialPtr);
            }
        } catch (std::exception&) {
            debugLog("Mesh " + currentMesh->GetName() + " has no primitives");
        }
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
            debugLog("Node " + newNode->GetName() + " has no name property");
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
            debugLog("Node " + newNode->GetName() + " has no matrix property");
        }
        try {
            const auto& position(node["translation"].GetArray());
            glm::vec3 positionVec3;
            positionVec3[0] = position[0].GetFloat();
            positionVec3[1] = position[1].GetFloat();
            positionVec3[2] = position[2].GetFloat();
            transform->SetPosition(positionVec3);
        } catch (std::exception&) {
            debugLog("Node " + newNode->GetName() + " has no translation property");
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
            debugLog("Node " + newNode->GetName() + " has no rotation property");
        }
        try {
            const auto& scale(node["scale"].GetArray());
            glm::vec3 scaleVec3;
            scaleVec3[0] = scale[0].GetFloat();
            scaleVec3[1] = scale[1].GetFloat();
            scaleVec3[2] = scale[2].GetFloat();
            transform->SetScale(scaleVec3);
        } catch (std::exception&) {
            debugLog("Node " + newNode->GetName() + " has no scale property");
        }
        nodeVector.push_back(newNode);
        nodeIndex++;
    }
    return nodeVector;
}

static inline auto ParseAnimations(const rapidjson::Document& document, const std::vector<std::shared_ptr<Node>>& nodes, const std::vector<std::shared_ptr<BufferAccessor>>& bufferAccessors)
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
                auto samplerInput(bufferAccessors.at(sampler["input"].GetInt()));
                auto samplerOutput(bufferAccessors.at(sampler["output"].GetInt()));
                samplerInput->GetBufferView()->SetStorage(BufferView::Storage::CPU);
                samplerOutput->GetBufferView()->SetStorage(BufferView::Storage::CPU);
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
                    newChannel.SetTarget(nodes.at(target["node"].GetInt()));
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

static inline auto ParseSkins(const rapidjson::Document& document, const std::vector<std::shared_ptr<Node>>& nodes, const std::vector<std::shared_ptr<BufferAccessor>>& bufferAccessors)
{
    debugLog("Start parsing Skins");
    std::vector<std::shared_ptr<MeshSkin>> skins;
    try {
        for (const auto& skin : document["skins"].GetArray()) {
            auto newSkin(Component::Create<MeshSkin>());
            try {
                newSkin->SetName(skin["name"].GetString());
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->GetName() + " has no name");
            }
            try {
                auto accessor { bufferAccessors.at(skin["inverseBindMatrices"].GetInt()) };
                accessor->GetBufferView()->SetStorage(BufferView::Storage::CPU);
                newSkin->SetInverseBindMatrices(accessor);
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->GetName() + " has no inverseBindMatrices");
            }
            /*try {
                newSkin->SetSkeleton(container->GetComponents<Node>().at(skin["skeleton"].GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->GetName() + " has no skeleton");
            }*/
            try {
                for (const auto& joint : skin["joints"].GetArray())
                    newSkin->AddJoint(nodes.at(joint.GetInt()));
            } catch (std::exception&) {
                debugLog("Skin " + newSkin->GetName() + " has no joints");
            }
            skins.push_back(newSkin);
        }
    } catch (std::exception&) {
        debugLog("No skins found");
    }
    debugLog("Done parsing Skins");
    return skins;
}

static inline auto SetParenting(const rapidjson::Document& document,
    const std::vector<std::shared_ptr<Node>>& nodes,
    const std::vector<std::shared_ptr<Mesh>>& meshes,
    const std::vector<std::shared_ptr<MeshSkin>>& meshSkins,
    const std::vector<std::shared_ptr<Camera>>& cameras)
{
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return;
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : nodeItr->value.GetArray()) {
        auto node(nodes.at(nodeIndex));
        try {
            auto mesh(meshes.at(gltfNode["mesh"].GetInt()));
            node->SetComponent(mesh);
            try {
                mesh->SetComponent(meshSkins.at(gltfNode["skin"].GetInt()));
            } catch (std::exception&) {
                debugLog("Mesh " + mesh->GetName() + " has no skin");
            }
        } catch (std::exception&) {
            debugLog("Node " + node->GetName() + " has no mesh");
        }
        try {
            auto camera = cameras.at(gltfNode["camera"].GetInt());
            //camera->SetParent(node);
            node->AddChild(camera);
            //container->GetComponents<Camera>().at(gltfNode["camera"].GetInt())->SetParent(node);
        } catch (std::exception&) {
            debugLog("Node " + node->GetName() + " has no camera");
        }
        try {
            for (const auto& child : gltfNode["children"].GetArray()) {
                auto childNode = nodes.at(child.GetInt());
                //childNode->SetParent(node);
                node->AddChild(nodes.at(child.GetInt()));
                //container->GetComponents<Node>().at(child.GetInt())->SetParent(node);
                std::cout << "Node parenting " << node->GetName() << " -> " << nodes.at(child.GetInt())->GetName() << std::endl;
            }
        } catch (std::exception&) {
            debugLog("Node " + node->GetName() + " has no skeleton");
        }
        nodeIndex++;
    }
}

static inline auto ParseScenes(
    const rapidjson::Document& document,
    const std::vector<std::shared_ptr<Node>>& nodes,
    const std::vector<std::shared_ptr<Animation>>& animations)
{
    std::vector<std::shared_ptr<Scene>> sceneVector;
    auto scenes(document["scenes"].GetArray());
    int sceneIndex = 0;
    for (const auto& scene : scenes) {
        std::cout << "found scene" << std::endl;
        auto newScene(Component::Create<Scene>(std::to_string(sceneIndex)));
        for (const auto& node : scene["nodes"].GetArray()) {
            newScene->AddRootNode(nodes.at(node.GetInt()));
            std::cout << nodes.at(node.GetInt())->GetName() << std::endl;
        }
        for (const auto& animation : animations) {
            newScene->Add(animation);
            std::cout << (animation ? animation->GetName() : "nullptr") << std::endl;
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

static inline auto ParseLights(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Light>> lightsVector;
    const auto& extensions(document.FindMember("extensions"));
    if (extensions != document.MemberEnd()) {
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
                    } catch (std::exception&) {
                        debugLog("[MISSING REQUIRED VALUE] : Light has no type");
                    }
                }
            }
        }
    }
    return lightsVector;
}

static inline auto ParseImages(const std::filesystem::path path, const rapidjson::Document& document, std::vector<std::shared_ptr<BufferView>>& bufferViews)
{
    std::vector<std::shared_ptr<Asset>> imagesVector;
    auto imagesItr(document.FindMember("images"));
    if (imagesItr == document.MemberEnd())
        return imagesVector;
    for (const auto& gltfImagee : imagesItr->value.GetArray()) {
        auto imageUriItr(gltfImagee.FindMember("uri"));
        if (imageUriItr == gltfImagee.MemberEnd()) {
            auto imageAsset { Component::Create<Asset>() };
            auto imageBufferViewItr(gltfImagee.FindMember("bufferView"));
            if (imageBufferViewItr == gltfImagee.MemberEnd()) {
                imageAsset->SetComponent(Component::Create<Image>(glm::ivec2(1), Pixel::SizedFormat::Uint8_NormalizedRGB));
                imageAsset->SetLoaded(true);
            } else {
                imageAsset->SetUri(std::string("data:") + gltfImagee["mimeType"].GetString() + ",");
                auto bufferViewIndex { imageBufferViewItr->value.GetInt() };
                imageAsset->AddComponent(bufferViews.at(bufferViewIndex));
                bufferViews.at(bufferViewIndex)->SetStorage(BufferView::Storage::CPU);
            }
            //TODO : learn how to use bufferView and mimeType
            imagesVector.push_back(imageAsset);
            continue;
        }
        auto uri = CreateUri(path.parent_path(), imageUriItr->value.GetString());
        imagesVector.push_back(Component::Create<Asset>(uri));
    }
    return imagesVector;
}

void ParseGLTF(std::shared_ptr<Asset> container)
{
    auto path = container->GetUri().GetPath();
    std::cout << path << std::endl;
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file !");
        return;
        //return container;
    }
    auto nodes { ParseNodes(document) };
    auto cameras { ParseCameras(document) };
    auto buffers { ParseBuffers(path, document) };
    auto bufferViews { ParseBufferViews(document, buffers) };
    auto bufferAccessors { ParseBufferAccessors(document, bufferViews) };
    auto images { ParseImages(path, document, bufferViews) };
    auto textures { ParseTextures(document, images) };
    auto materials { ParseMaterials(document, textures) };
    auto meshes { ParseMeshes(document, materials, bufferAccessors) };
    auto skins { ParseSkins(document, nodes, bufferAccessors) };
    auto animations { ParseAnimations(document, nodes, bufferAccessors) };
    auto scenes { ParseScenes(document, nodes, animations) };
    for (const auto& node : nodes)
        container->AddComponent(node);
    for (const auto& camera : cameras)
        container->AddComponent(camera);
    for (const auto& buffer : buffers)
        container->AddComponent(buffer);
    for (const auto& bufferView : bufferViews)
        container->AddComponent(bufferView);
    for (const auto& bufferAccessor : bufferAccessors)
        container->AddComponent(bufferAccessor);
    for (const auto& image : images)
        container->AddComponent(image);
    for (const auto& texture : textures)
        container->AddComponent(texture);
    for (const auto& material : materials)
        container->AddComponent(material);
    for (const auto& meshe : meshes)
        container->AddComponent(meshe);
    for (const auto& skin : skins)
        container->AddComponent(skin);
    for (const auto& animation : animations)
        container->AddComponent(animation);
    for (const auto& scene : scenes)
        container->AddComponent(scene);
    SetParenting(document, nodes, meshes, skins, cameras);
    container->SetLoaded(true);
}