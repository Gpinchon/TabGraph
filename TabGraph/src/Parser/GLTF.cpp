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
#include "Surface/Geometry.hpp"
#include "Surface/Mesh.hpp"
#include "Surface/MeshSkin.hpp"
#include "Parser/InternalTools.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Texture2D.hpp"
#include "Texture/TextureSampler.hpp"

#include <filesystem>
#include <fstream>
#include <glm/ext.hpp>
#include <iostream>
#include <memory>

void ParseGLTF(std::shared_ptr<Asset>);

auto GLTFMimeExtension {
    AssetsParser::AddMimeExtension("model/gltf+json", ".gltf")
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

static inline auto ParseCameras(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Camera>> cameraVector;
    if (!document.HasMember("cameras")) return cameraVector;
    auto cameraIndex(0);
    for (const auto& camera : document["cameras"].GetArray()) {
        auto newCamera(Component::Create<Camera>("Camera" + std::to_string(cameraIndex)));
        if (std::string(camera["type"].GetString()) == "perspective") {
            auto perspective(camera["perspective"].GetObject());
            if (perspective.HasMember("zfar")) {
                Camera::Projection::Perspective projection;
                projection.zfar = perspective["zfar"].GetFloat();
                if (perspective.HasMember("znear"))
                    projection.znear = perspective["znear"].GetFloat();
                if (perspective.HasMember("yfov"))
                    projection.fov = glm::degrees(perspective["yfov"].GetFloat());
                newCamera->SetProjection(projection);
            }
            else {
                Camera::Projection::PerspectiveInfinite projection;
                if (perspective.HasMember("znear"))
                    projection.znear = perspective["znear"].GetFloat();
                if (perspective.HasMember("yfov"))
                    projection.fov = glm::degrees(perspective["yfov"].GetFloat());
                newCamera->SetProjection(projection);
            }
        }
        else if (std::string(camera["type"].GetString()) == "orthographic") {
            Camera::Projection::Orthographic projection;
            newCamera->SetProjection(projection);
        }
        cameraVector.push_back(newCamera);
        cameraIndex++;
    }
    return cameraVector;
}

auto GetFilter(int filter) {
    switch (filter) {
    case 9728:
        return TextureSampler::Filter::Nearest;
    case 9729:
        return TextureSampler::Filter::Linear;
    case 9984:
        return TextureSampler::Filter::NearestMipmapNearest;
    case 9985:
        return TextureSampler::Filter::LinearMipmapNearest;
    case 9986:
        return TextureSampler::Filter::NearestMipmapLinear;
    case 9987:
        return TextureSampler::Filter::LinearMipmapLinear;
    default:
        throw std::runtime_error("Unknown Texture filter");
    }
}

auto GetWrap(int wrap) {
    switch (wrap)
    {
    case 33071:
        return TextureSampler::Wrap::ClampToEdge;
    case 33648:
        return TextureSampler::Wrap::MirroredRepeat;
    case 10497:
        return TextureSampler::Wrap::Repeat;
    default:
        throw std::runtime_error("Unknown Texture Wrap mode");
    }
}

static inline auto ParseTextureSamplers(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<TextureSampler>> samplerVector;
    if (!document.HasMember("samplers")) return samplerVector;
    for (const auto& sampler : document["samplers"].GetArray()) {
        auto newSampler{ std::make_shared<TextureSampler>() };
        for (rapidjson::Value::ConstMemberIterator setting = sampler.MemberBegin(); setting != sampler.MemberEnd(); setting++) {
            if ("magFilter" == setting->name.GetString())
                newSampler->SetMagFilter(GetFilter(setting->value.GetInt()));
            else if ("minFilter" == setting->name.GetString())
                newSampler->SetMinFilter(GetFilter(setting->value.GetInt()));
            else if ("wrapS" == setting->name.GetString())
                newSampler->SetWrapS(GetWrap(setting->value.GetInt()));
            else if ("wrapT" == setting->name.GetString())
                newSampler->SetWrapT(GetWrap(setting->value.GetInt()));
        }
        samplerVector.push_back(newSampler);
    }
    return samplerVector;
}

static inline auto ParseTextures(const rapidjson::Document& document, std::vector<std::shared_ptr<Asset>>& images, std::shared_ptr<Asset> container)
{
    debugLog("Start parsing textures");
    std::vector<std::shared_ptr<Texture2D>> textureVector;
    if (!document.HasMember("textures")) return textureVector;
    auto samplers = ParseTextureSamplers(document);
    auto textureIndex(0);
    for (const auto& textureValue : document["textures"].GetArray()) {
        std::shared_ptr<Texture2D> texture = nullptr;
        if (textureValue.HasMember("source")) {
            auto source(textureValue["source"].GetInt());
            auto image { images.at(source) };
            texture = Component::Create<Texture2D>(image);
        }
        if (textureValue.HasMember("sampler")) {
            auto sampler(samplers.at(textureValue["sampler"].GetInt()));
            texture->SetTextureSampler(sampler);
        }
        texture->SetCompressed(container->parsingOptions.texture.compress);
        texture->SetCompressionQuality(container->parsingOptions.texture.compressionQuality);
        textureVector.push_back(texture);
        textureIndex++;
    }
    debugLog("Done parsing textures");
    return textureVector;
}

#include "Material/MetallicRoughness.hpp"
#include "Material/SpecularGlossiness.hpp"

static inline auto ParseMaterialExtensions(const std::vector<std::shared_ptr<Texture2D>>& textures, const rapidjson::Value& materialValue, std::shared_ptr<Material> material)
{
    if (!materialValue.HasMember("extensions")) return;
    for (const auto& extension : materialValue["extensions"].GetObject()) {
        if (std::string(extension.name.GetString()) == "KHR_materials_pbrSpecularGlossiness") {
            const auto& pbrSpecularGlossiness = extension.value;
            auto materialExtension = Component::Create<SpecularGlossiness>();
            material->AddExtension(materialExtension);
            if (pbrSpecularGlossiness.HasMember("diffuseFactor")) {
                auto diffuseFactor(pbrSpecularGlossiness["diffuseFactor"].GetArray());
                materialExtension->SetDiffuse(glm::vec3(
                    diffuseFactor[0].GetFloat(),
                    diffuseFactor[1].GetFloat(),
                    diffuseFactor[2].GetFloat()));
                materialExtension->SetOpacity(diffuseFactor[3].GetFloat());
            }
            if (pbrSpecularGlossiness.HasMember("specularFactor")) {
                auto diffuseFactor(pbrSpecularGlossiness["specularFactor"].GetArray());
                materialExtension->SetSpecular(glm::vec3(
                    diffuseFactor[0].GetFloat(),
                    diffuseFactor[1].GetFloat(),
                    diffuseFactor[2].GetFloat()));
            }
            if (pbrSpecularGlossiness.HasMember("glossinessFactor")) {
                auto glossinessFactor(pbrSpecularGlossiness["glossinessFactor"].GetFloat());
                materialExtension->SetGlossiness(glossinessFactor);
            }
            if (pbrSpecularGlossiness.HasMember("diffuseTexture")) {
                auto textureObject(pbrSpecularGlossiness["diffuseTexture"].GetObject());
                materialExtension->SetTextureDiffuse(textures.at(textureObject["index"].GetInt()));
            }
            if (pbrSpecularGlossiness.HasMember("specularGlossinessTexture")) {
                auto textureObject(pbrSpecularGlossiness["specularGlossinessTexture"].GetObject());
                materialExtension->SetTextureSpecularGlossiness(textures.at(textureObject["index"].GetInt()));
            }
        }
    }
}

static inline auto ParseMaterials(const rapidjson::Document& document, std::vector<std::shared_ptr<Texture2D>>& textures)
{
    debugLog("Start parsing materials");
    //auto textureVector = ParseTextures(path, document);
    std::vector<std::shared_ptr<Material>> materialVector;
    if (!document.HasMember("materials")) return materialVector;
    auto materialIndex(0);
    for (const auto& materialValue : document["materials"].GetArray()) {
        auto material(Component::Create<Material>("Material " + std::to_string(materialIndex)));
        if (materialValue.HasMember("name"))
            material->SetName(materialValue["name"].GetString());
        if (materialValue.HasMember("alphaCutoff"))
            material->SetOpacityCutoff(materialValue["alphaCutoff"].GetFloat());
        if (materialValue.HasMember("alphaMode")) {
            std::string alphaMode = materialValue["alphaMode"].GetString();
            if (alphaMode == "Opaque")
                material->SetOpacityMode(Material::OpacityMode::Opaque);
            if (alphaMode == "MASK")
                material->SetOpacityMode(Material::OpacityMode::Mask);
            if (alphaMode == "BLEND")
                material->SetOpacityMode(Material::OpacityMode::Blend);
        }

        if (materialValue.HasMember("doubleSided"))
            material->SetDoubleSided(materialValue["doubleSided"].GetBool());
        if (materialValue.HasMember("emissiveFactor")) {
            auto emissiveFactor(materialValue["emissiveFactor"].GetArray());
            material->SetEmissive(glm::vec3(emissiveFactor[0].GetFloat(),
                emissiveFactor[1].GetFloat(),
                emissiveFactor[2].GetFloat()));
        }
        if (materialValue.HasMember("normalTexture")) {
            auto textureObject(materialValue["normalTexture"].GetObject());
            material->SetTextureNormal(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("emissiveTexture")) {
            auto textureObject(materialValue["emissiveTexture"].GetObject());
            material->SetTextureEmissive(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("occlusionTexture")) {
            auto textureObject(materialValue["occlusionTexture"].GetObject());
            material->SetTextureAO(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("pbrMetallicRoughness")) {
            auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
            auto materialExtension = Component::Create<MetallicRoughness>();
            material->AddExtension(materialExtension);
            materialExtension->SetRoughness(1);
            materialExtension->SetMetallic(1);
            if (pbrMetallicRoughness.HasMember("metallicRoughnessTexture")) {
                auto textureObject(pbrMetallicRoughness["metallicRoughnessTexture"].GetObject());
                materialExtension->SetTextureMetallicRoughness(textures.at(textureObject["index"].GetInt()));
            }
            if (pbrMetallicRoughness.HasMember("baseColorFactor")) {
                auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
                materialExtension->SetBaseColor(glm::vec3(baseColor[0].GetFloat(),
                    baseColor[1].GetFloat(),
                    baseColor[2].GetFloat()));
                materialExtension->SetOpacity(baseColor[3].GetFloat());
            }
            if (pbrMetallicRoughness.HasMember("metallicFactor"))
                materialExtension->SetMetallic(pbrMetallicRoughness["metallicFactor"].GetFloat());
            if (pbrMetallicRoughness.HasMember("roughnessFactor"))
                materialExtension->SetRoughness(pbrMetallicRoughness["roughnessFactor"].GetFloat());
            if (pbrMetallicRoughness.HasMember("baseColorTexture")) {
                auto textureObject(pbrMetallicRoughness["baseColorTexture"].GetObject());
                materialExtension->SetTextureBaseColor(textures.at(textureObject["index"].GetInt()));
            }
        }
        ParseMaterialExtensions(textures, materialValue, material);
        materialVector.push_back(material);
        materialIndex++;
    }
    debugLog("Done parsing materials");
    return materialVector;
}

static inline auto ParseBuffers(const std::filesystem::path path, const rapidjson::Document& document, std::shared_ptr<Asset> container)
{
    debugLog("Start parsing buffers");
    std::vector<std::shared_ptr<Asset>> bufferVector;
    if (!document.HasMember("buffers")) return bufferVector;
    for (const auto& bufferValue : document["buffers"].GetArray()) {
        auto asset { Component::Create<Asset>() };
        asset->parsingOptions = container->parsingOptions;
        auto uri = CreateUri(path.parent_path(), bufferValue["uri"].GetString());
        asset->SetUri(uri);
        if (bufferValue.HasMember("name"))
            asset->SetName(bufferValue["name"].GetString());
        bufferVector.push_back(asset);
    }
    debugLog("Done parsing buffers");
    return bufferVector;
}

static inline BufferView::Type GetBufferViewType(unsigned type)
{
    switch (type) {
    case 34962:
        return BufferView::Type::Array;
    case 34963:
        return BufferView::Type::ElementArray;
    default:
        return BufferView::Type::Unknown;
    }
}

static inline auto ParseBufferViews(const rapidjson::Document& document, std::vector<std::shared_ptr<Asset>> buffers)
{
    debugLog("Start parsing bufferViews");
    std::vector<std::shared_ptr<BufferView>> bufferViewVector;
    if (!document.HasMember("bufferViews")) return bufferViewVector;
    for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
        auto bufferView(Component::Create<BufferView>(
            bufferViewValue["byteLength"].GetInt(),
            buffers.at(bufferViewValue["buffer"].GetInt())));
        if (bufferViewValue.HasMember("name"))
            bufferView->SetName(bufferViewValue["name"].GetString());
        if (bufferViewValue.HasMember("byteOffset"))
            bufferView->SetByteOffset(bufferViewValue["byteOffset"].GetInt());
        if (bufferViewValue.HasMember("byteStride"))
            bufferView->SetByteStride(bufferViewValue["byteStride"].GetInt());
        if (bufferViewValue.HasMember("target"))
            bufferView->SetType(GetBufferViewType(bufferViewValue["target"].GetInt()));
        bufferViewVector.push_back(bufferView);
    }
    debugLog("Done parsing bufferViews");
    return bufferViewVector;
}

static inline BufferAccessor::ComponentType GetBufferAccessorComponentType(unsigned type)
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
    if (!document.HasMember("accessors"))
        return bufferAccessorVector;
    auto bufferAccessorIndex(0);
    for (const auto& bufferAccessorValue : document["accessors"].GetArray()) {
        auto bufferAccessor(Component::Create<BufferAccessor>(
            GetBufferAccessorComponentType(bufferAccessorValue["componentType"].GetInt()),
            GetBufferAccessorType(bufferAccessorValue["type"].GetString()),
            bufferAccessorValue["count"].GetInt()));
        bufferAccessor->SetName("BufferAccessor " + std::to_string(bufferAccessorIndex));
        if (bufferAccessorValue.HasMember("name"))
            bufferAccessor->SetName(bufferAccessorValue["name"].GetString());
        if (bufferAccessorValue.HasMember("bufferView"))
            bufferAccessor->SetBufferView(bufferViews.at(bufferAccessorValue["bufferView"].GetInt()));
        if (bufferAccessorValue.HasMember("byteOffset"))
            bufferAccessor->SetByteOffset(bufferAccessorValue["byteOffset"].GetInt());
        if (bufferAccessorValue.HasMember("normalized"))
            bufferAccessor->SetNormalized(bufferAccessorValue["normalized"].GetBool());
        /*try {
            bufferAccessor->SetCount(bufferAccessorValue["count"].GetInt());
        } catch (std::exception&) {
            debugLog("Accessor " + bufferAccessor->GetName() + " has no count property")
        }*/
        bufferAccessorVector.push_back(bufferAccessor);
        bufferAccessorIndex++;
    }
    debugLog("Done parsing bufferAccessors");
    return bufferAccessorVector;
}

static inline Geometry::DrawingMode GetGeometryDrawingMode(unsigned mode)
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
        if (mesh.HasMember("name"))
            currentMesh->SetName(mesh["name"].GetString());
        if (mesh.HasMember("primitives")) {
            for (const auto& primitive : mesh["primitives"].GetArray()) {
                debugLog("Found new primitive");
                auto geometry(Component::Create<Geometry>());
                auto materialPtr{ defaultMaterial };
                if (primitive.HasMember("material")) {
                    auto& material(primitive["material"]);
                    if (size_t(material.GetInt()) >= materials.size())
                        std::cerr << "Material index " << material.GetInt() << " out of bound " << materials.size() << std::endl;
                    materialPtr = materials.at(material.GetInt());
                }
                if (primitive.HasMember("attributes")) {
                    for (const auto& attribute : primitive["attributes"].GetObject()) {
                        auto attributeName(std::string(attribute.name.GetString()));
                        auto accessor(bufferAccessors.at(attribute.value.GetInt()));
                        auto accessorKey(Geometry::GetAccessorKey(attributeName));
                        if (accessorKey == Geometry::AccessorKey::Invalid) {
                            debugLog("Invalid Accessor Key : " + attributeName);
                        }
                        else {
                            geometry->SetAccessor(accessorKey, accessor);
                            accessor->GetBufferView()->SetType(BufferView::Type::Array);
                        }
                    }
                }
                if (primitive.HasMember("indices")) {
                    auto accessor(bufferAccessors.at(primitive["indices"].GetInt()));
                    geometry->SetIndices(accessor);
                    accessor->GetBufferView()->SetType(BufferView::Type::ElementArray);
                }
                if (primitive.HasMember("mode"))
                    geometry->SetDrawingMode(GetGeometryDrawingMode(primitive["mode"].GetInt()));
                currentMesh->AddGeometry(geometry, materialPtr);
            }
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
        if (node.HasMember("name"))
            newNode->SetName(node["name"].GetString());
        if (node.HasMember("matrix")) {
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
        }
        if (node.HasMember("translation")) {
            const auto& position(node["translation"].GetArray());
            glm::vec3 positionVec3;
            positionVec3[0] = position[0].GetFloat();
            positionVec3[1] = position[1].GetFloat();
            positionVec3[2] = position[2].GetFloat();
            transform->SetPosition(positionVec3);
        }
        if (node.HasMember("rotation")) {
            const auto& rotation(node["rotation"].GetArray());
            glm::quat rotationQuat;
            rotationQuat[0] = rotation[0].GetFloat();
            rotationQuat[1] = rotation[1].GetFloat();
            rotationQuat[2] = rotation[2].GetFloat();
            rotationQuat[3] = rotation[3].GetFloat();
            transform->SetRotation(glm::normalize(rotationQuat));
        }
        if (node.HasMember("scale")) {
            const auto& scale(node["scale"].GetArray());
            glm::vec3 scaleVec3;
            scaleVec3[0] = scale[0].GetFloat();
            scaleVec3[1] = scale[1].GetFloat();
            scaleVec3[2] = scale[2].GetFloat();
            transform->SetScale(scaleVec3);
        }
        nodeVector.push_back(newNode);
        nodeIndex++;
    }
    return nodeVector;
}

static inline auto ParseAnimations(const rapidjson::Document& document, const std::vector<std::shared_ptr<Node>>& nodes, const std::vector<std::shared_ptr<BufferAccessor>>& bufferAccessors)
{
    std::vector<std::shared_ptr<Animation>> animations;
    if (!document.HasMember("animations"))
        return animations;
    for (const auto& animation : document["animations"].GetArray()) {
        auto newAnimation(Component::Create<Animation>());
        if (animation.HasMember("name"))
            newAnimation->SetName(animation["name"].GetString());
        for (const auto& sampler : animation["samplers"].GetArray()) {
            auto samplerInput(bufferAccessors.at(sampler["input"].GetInt()));
            auto samplerOutput(bufferAccessors.at(sampler["output"].GetInt()));
            samplerInput->GetBufferView()->SetStorage(BufferView::Storage::CPU);
            samplerOutput->GetBufferView()->SetStorage(BufferView::Storage::CPU);
            auto newSampler(AnimationSampler(samplerInput, samplerOutput));
            if (sampler.HasMember("interpolation")) {
                std::string interpolation(sampler["interpolation"].GetString());
                if (interpolation == "LINEAR")
                    newSampler.SetInterpolation(AnimationSampler::AnimationInterpolation::Linear);
                else if (interpolation == "STEP")
                    newSampler.SetInterpolation(AnimationSampler::AnimationInterpolation::Step);
                else if (interpolation == "CUBICSPLINE")
                    newSampler.SetInterpolation(AnimationSampler::AnimationInterpolation::CubicSpline);
            }
            newAnimation->AddSampler(newSampler);
        }
        for (const auto& channel : animation["channels"].GetArray()) {
            AnimationChannel newChannel;
            if (channel.HasMember("target")) {
                auto& target = (channel["target"]);
                if (target.HasMember("path")) {
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
                }
            }
            newAnimation->AddChannel(newChannel);
        }
        animations.push_back(newAnimation);
    }
    return animations;
}

static inline auto ParseSkins(const rapidjson::Document& document, const std::vector<std::shared_ptr<Node>>& nodes, const std::vector<std::shared_ptr<BufferAccessor>>& bufferAccessors)
{
    debugLog("Start parsing Skins");
    std::vector<std::shared_ptr<MeshSkin>> skins;
    if (!document.HasMember("skins"))
        return skins;
    for (const auto& skin : document["skins"].GetArray()) {
        auto newSkin(Component::Create<MeshSkin>());
        if (skin.HasMember("name"))
            newSkin->SetName(skin["name"].GetString());
        if (skin.HasMember("inverseBindMatrices")) {
            auto accessor{ bufferAccessors.at(skin["inverseBindMatrices"].GetInt()) };
            accessor->GetBufferView()->SetStorage(BufferView::Storage::CPU);
            newSkin->SetInverseBindMatrices(accessor);
        }
        /*try {
            newSkin->SetSkeleton(container->GetComponents<Node>().at(skin["skeleton"].GetInt()));
        } catch (std::exception&) {
            debugLog("Skin " + newSkin->GetName() + " has no skeleton");
        }*/
        if (skin.HasMember("joints")) {
            for (const auto& joint : skin["joints"].GetArray())
                newSkin->AddJoint(nodes.at(joint.GetInt()));
        }
        skins.push_back(newSkin);
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
        if (gltfNode.HasMember("mesh")) {
            auto mesh(meshes.at(gltfNode["mesh"].GetInt()));
            node->SetComponent<Surface>(mesh);
            if (gltfNode.HasMember("skin"))
                mesh->SetComponent(meshSkins.at(gltfNode["skin"].GetInt()));
        }
        if (gltfNode.HasMember("camera")) {
            auto camera = cameras.at(gltfNode["camera"].GetInt());
            node->AddChild(camera);
        }
        if (gltfNode.HasMember("children")) {
            for (const auto& child : gltfNode["children"].GetArray()) {
                auto childNode = nodes.at(child.GetInt());
                node->AddChild(nodes.at(child.GetInt()));
                std::cout << "Node parenting " << node->GetName() << " -> " << nodes.at(child.GetInt())->GetName() << std::endl;
            }
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

#include "Light/DirectionalLight.hpp"

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
                    if (light.HasMember("type")) {
                        auto type = light["type"].GetString();
                        if (type == "directional")
                            newLight = Component::Create<DirectionalLight>();
                        lightsVector.emplace_back(newLight);
                    }
                }
            }
        }
    }
    return lightsVector;
}

static inline auto ParseImages(const std::filesystem::path path, const rapidjson::Document& document, std::vector<std::shared_ptr<BufferView>>& bufferViews, std::shared_ptr<Asset> container)
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
            imageAsset->parsingOptions = container->parsingOptions;
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
        auto imageAsset = Component::Create<Asset>(uri);
        imageAsset->parsingOptions = container->parsingOptions;
        imagesVector.push_back(imageAsset);
    }
    return imagesVector;
}

void ParseGLTF(std::shared_ptr<Asset> container)
{
    auto path = container->GetUri().DecodePath();
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
    auto buffers { ParseBuffers(path, document, container) };
    auto bufferViews { ParseBufferViews(document, buffers) };
    auto bufferAccessors { ParseBufferAccessors(document, bufferViews) };
    auto images { ParseImages(path, document, bufferViews, container) };
    auto textures { ParseTextures(document, images, container) };
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