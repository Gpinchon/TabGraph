/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 16:52:34
*/

#include "Parser/FBX.hpp"
#include "Assets/AssetsParser.hpp"
#include "Material.hpp"
#include "Mesh/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Parser/FBX/FBXDocument.hpp"
#include "Parser/FBX/FBXNode.hpp"
#include "Parser/FBX/FBXObject.hpp"
#include "Parser/FBX/FBXProperty.hpp"
#include "Scene/Scene.hpp"
#include "Transform.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

//Add this parser to SceneParser !
auto __fbxParser = AssetsParser::Add(".fbx", FBX::Parse);

/*static inline std::vector<glm::vec2> parseUV(FBX::Node *layerElementUV)
{
    std::vector<glm::vec2> uv;
    if (layerElementUV == nullptr)
        return uv;
    layerElementUV->SubNode("UV")->Print();
    auto UV = layerElementUV->SubNode("UV");
    if (UV == nullptr)
        return uv;
    FBX::Array UVArray(UV->Property(0));
    for (auto i = 0u; i < UVArray.length / 2; i++)
    {
        auto vec2 = glm::vec2(
            std::get<double *>(UVArray.data)[i * 2 + 0],
            std::get<double *>(UVArray.data)[i * 2 + 1]);
        uv.push_back(vec2);
    }
    auto UVIndex = layerElementUV->SubNode("UVIndex");
    if (UVIndex != nullptr)
    {
        std::vector<glm::vec2> realUV;
        FBX::Array UVIndexArray(UVIndex->Property(0));
        for (auto i = 0u; i < UVIndexArray.length; i++)
        {
            auto index = std::get<int32_t *>(UVIndexArray.data)[i];
            realUV.push_back(uv.at(index));
        }
        return realUV;
    }
    return uv;
}*/

static inline auto getMaterials(std::shared_ptr<FBX::Node> layerElementMaterial)
{
    std::vector<int32_t> materials;
    if (layerElementMaterial == nullptr)
        return materials;
    auto materialsNode(layerElementMaterial->SubNode("Materials"));
    FBX::Array materialArray(materialsNode->Property(0));
    for (auto i = 0u; i < materialArray.length; i++) {
        auto index(((int32_t*)materialArray)[i]);
        materials.push_back(index);
    }
    auto referenceInformationType(layerElementMaterial->SubNode("ReferenceInformationType"));
    std::string referenceType(referenceInformationType ? std::string(referenceInformationType->Property(0)) : std::string(""));
    if (referenceType == "" || referenceType == "Direct") //Else mapping is DirectToIndirect
        return materials;
    auto materialsIndex(layerElementMaterial->SubNode("Materials"));
    if (materialsIndex == nullptr)
        throw std::runtime_error("Node(" + layerElementMaterial->Name() + ") missing Materials.");
    auto materialsIndexArray(FBX::Array(materialsIndex->Property(0)));
    std::vector<int32_t> materialsUnindexed;
    materialsUnindexed.reserve(materials.size());
    for (auto i = 0u; i < materialsIndexArray.length; i++) {
        auto index = ((int32_t*)materialsIndexArray)[i];
        materialsUnindexed.push_back(materials.at(index));
    }
    return materialsUnindexed;
}

static inline auto getNormals(std::shared_ptr<FBX::Node> layerElementNormal)
{
    std::vector<glm::vec3> vn;
    if (layerElementNormal == nullptr)
        return vn;
    auto normals(layerElementNormal->SubNode("Normals"));
    FBX::Array vnArray(normals->Property(0));
    for (auto i = 0u; i < vnArray.length / 3; i++) {
        glm::vec3 nd(
            std::get<double*>(vnArray.data)[i * 3 + 0],
            std::get<double*>(vnArray.data)[i * 3 + 1],
            std::get<double*>(vnArray.data)[i * 3 + 2]);
        //nd = (nd + 1.f) * 0.5f * 255.f;
        //glm::vec3 n(nd, 255);
        vn.push_back(nd);
    }
    auto referenceInformationType(layerElementNormal->SubNode("ReferenceInformationType"));
    std::string referenceType(referenceInformationType ? std::string(referenceInformationType->Property(0)) : std::string(""));
    if (referenceType == "" || referenceType == "Direct")
        return vn;
    auto normalsIndex(layerElementNormal->SubNode("NormalsIndex"));
    if (normalsIndex == nullptr)
        throw std::runtime_error("Node(" + layerElementNormal->Name() + ") missing NormalsIndex.");
    auto normalsIndexArray(FBX::Array(normalsIndex->Property(0)));
    std::vector<glm::vec3> vnUnindexed;
    vnUnindexed.reserve(vn.size());
    for (auto i = 0u; i < normalsIndexArray.length; i++) {
        auto index = ((int32_t*)normalsIndexArray)[i];
        vnUnindexed.push_back(vn.at(index));
    }
    return vnUnindexed;
}

static inline auto getVec3Vector(std::shared_ptr<FBX::Node> vertices)
{
    std::vector<glm::vec3> v;
    if (vertices == nullptr)
        return v;
    FBX::Array vArray(vertices->Property(0));
    for (auto i = 0u; i < vArray.length / 3; i++) {
        auto vec3 = glm::vec3(
            std::get<double*>(vArray.data)[i * 3 + 0],
            std::get<double*>(vArray.data)[i * 3 + 1],
            std::get<double*>(vArray.data)[i * 3 + 2]);
        v.push_back(vec3);
    }
    return v;
}

static inline auto getIntVector(std::shared_ptr<FBX::Node> polygonVertexIndex)
{
    std::vector<int32_t> vi;
    if (polygonVertexIndex == nullptr)
        return vi;
    FBX::Array viArray(polygonVertexIndex->Property(0));
    for (auto i = 0u; i < viArray.length; i++) {
        vi.push_back(std::get<int32_t*>(viArray.data)[i]);
    }
    return vi;
}

static inline auto getMappedIndices(const std::string& mappingInformationType, const std::vector<int32_t>& polygonIndices)
{
    std::vector<unsigned> indices;
    if (mappingInformationType == "ByPolygonVertex") {
        indices.resize(polygonIndices.size());
        for (auto i = 0u; i < polygonIndices.size(); i++) {
            indices.at(i) = i;
        }
    } else if (mappingInformationType == "AllSame") {
        indices.resize(polygonIndices.size());
        std::fill(indices.begin(), indices.end(), 0);
    } else if (mappingInformationType == "ByVertex" || mappingInformationType == "ByVertice") {
        indices.resize(polygonIndices.size());
        for (auto i = 0u; i < polygonIndices.size(); i++) {
            auto index = polygonIndices.at(i);
            index = index >= 0 ? index : abs(index) - 1;
            indices.at(i) = index;
            //indices.at(index) = i / 3;
        }
    } else if (mappingInformationType == "ByPolygon") {
        int normalIndex = 0;
        indices.resize(polygonIndices.size());
        for (auto i = 0u; i < polygonIndices.size(); i++) {
            auto index = polygonIndices.at(i);
            indices.at(i) = normalIndex;
            if (index < 0) {
                normalIndex++;
            }
        }
    }
    std::cout << "Generate Mapped Indices DONE" << std::endl;
    return indices;
}

#include "Buffer/BufferHelper.hpp"

AssetsContainer ParseGeometries(FBX::Document* document)
{
    AssetsContainer container;
    //std::map<int64_t, std::vector<std::shared_ptr<Geometry>>> groupMap;
    for (const auto& objects : document->SubNodes("Objects")) {
        for (const auto& geometryNode : objects->SubNodes("Geometry")) {
            if (geometryNode == nullptr)
                continue;
            int64_t geometryId(geometryNode->Property(0));
            std::cout << "Geometry ID " << geometryId << std::endl;
            auto vertices(getVec3Vector(geometryNode->SubNode("Vertices")));
            if (vertices.empty())
                continue;
            auto layerElementNormal(geometryNode->SubNode("LayerElementNormal"));
            auto layerElementMaterial(geometryNode->SubNode("LayerElementMaterial"));
            auto verticesIndices(getIntVector(geometryNode->SubNode("PolygonVertexIndex")));
            auto materials(getMaterials(layerElementMaterial));
            auto normals(getNormals(layerElementNormal));
            std::vector<unsigned> normalsIndices;
            if (layerElementNormal != nullptr)
                normalsIndices = getMappedIndices(layerElementNormal->SubNode("MappingInformationType")->Property(0), verticesIndices);
            std::vector<unsigned> materialIndices;
            if (layerElementMaterial != nullptr)
                materialIndices = getMappedIndices(layerElementMaterial->SubNode("MappingInformationType")->Property(0), verticesIndices);
            std::vector<int32_t> polygonIndex;
            std::vector<int32_t> polygonIndexNormal;
            std::shared_ptr<Geometry> geometry;
            //auto Geometry(Geometry::Create(""));
            //Geometry->SetId(geometryId);
            //groupMap[geometryId].push_back(Geometry);
            //auto bufferV = BufferHelper::CreateAccessor(vertices);
            //auto bufferN = BufferHelper::CreateAccessor(normals, GL_ARRAY_BUFFER, true);
            for (auto i = 0u; i < verticesIndices.size(); i++) {
                auto index = verticesIndices.at(i);
                polygonIndex.push_back(index >= 0 ? index : abs(index) - 1);
                polygonIndexNormal.push_back(normalsIndices.at(i));
                if (!materialIndices.empty()) {
                    uint32_t materialIndex(materials.at(materialIndices.at(i)));
                    if (geometry == nullptr) {
                        geometry = Geometry::Create("");
                        geometry->SetId(geometryId);
                        geometry->SetMaterialIndex(materialIndex);
                        geometry->SetAccessor(Geometry::AccessorKey::Position, BufferHelper::CreateAccessor<glm::vec3>(0));
                        geometry->SetAccessor(Geometry::AccessorKey::Normal, BufferHelper::CreateAccessor<glm::vec3>(0, GL_ARRAY_BUFFER, true));
                        //geometry->SetIndices(BufferHelper::CreateAccessor(0, GL_ELEMENT_ARRAY_BUFFER));
                        container.AddComponent(geometry);
                    } else if (materialIndex != geometry->MaterialIndex()) {
                        std::cout << "Material Indices : Last " << geometry->MaterialIndex() << " New " << materialIndex << std::endl;
                        std::cout << "Material index changed, create new Geometry" << std::endl;
                        geometry = Geometry::Create("");
                        geometry->SetId(geometryId);
                        geometry->SetMaterialIndex(materialIndex);
                        geometry->SetAccessor(Geometry::AccessorKey::Position, BufferHelper::CreateAccessor<glm::vec3>(0));
                        geometry->SetAccessor(Geometry::AccessorKey::Normal, BufferHelper::CreateAccessor<glm::vec3>(0, GL_ARRAY_BUFFER, true));
                        //geometry->SetIndices(BufferHelper::CreateAccessor(0, GL_ELEMENT_ARRAY_BUFFER));
                        container.AddComponent(geometry);
                    }
                }

                if (index < 0) {
                    if (geometry == nullptr) {
                        geometry = Geometry::Create("");
                        geometry->SetId(geometryId);
                        container.AddComponent(geometry);
                    }
                    BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Position), vertices.at(polygonIndex.at(0)));
                    BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Position), vertices.at(polygonIndex.at(1)));
                    BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Position), vertices.at(polygonIndex.at(2)));
                    BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Normal), normals.at(polygonIndexNormal.at(0)));
                    BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Normal), normals.at(polygonIndexNormal.at(1)));
                    BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Normal), normals.at(polygonIndexNormal.at(2)));
                    //Geometry->v.push_back(vertices.at(polygonIndex.at(0)));
                    //Geometry->v.push_back(vertices.at(polygonIndex.at(1)));
                    //Geometry->v.push_back(vertices.at(polygonIndex.at(2)));
                    //Geometry->vn.push_back(normals.at(polygonIndexNormal.at(0)));
                    //Geometry->vn.push_back(normals.at(polygonIndexNormal.at(1)));
                    //Geometry->vn.push_back(normals.at(polygonIndexNormal.at(2)));
                    if (polygonIndex.size() == 4) {
                        BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Position), vertices.at(polygonIndex.at(2)));
                        BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Position), vertices.at(polygonIndex.at(3)));
                        BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Position), vertices.at(polygonIndex.at(0)));
                        BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Normal), normals.at(polygonIndexNormal.at(2)));
                        BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Normal), normals.at(polygonIndexNormal.at(3)));
                        BufferHelper::PushBack(geometry->Accessor(Geometry::AccessorKey::Normal), normals.at(polygonIndexNormal.at(0)));
                        //Geometry->v.push_back(vertices.at(polygonIndex.at(2)));
                        //Geometry->v.push_back(vertices.at(polygonIndex.at(3)));
                        //Geometry->v.push_back(vertices.at(polygonIndex.at(0)));
                        //Geometry->vn.push_back(normals.at(polygonIndexNormal.at(2)));
                        //Geometry->vn.push_back(normals.at(polygonIndexNormal.at(3)));
                        //Geometry->vn.push_back(normals.at(polygonIndexNormal.at(0)));
                    }
                    polygonIndex.clear();
                    polygonIndexNormal.clear();
                    continue;
                }
            }
        }
    }
    return container;
}

static inline AssetsContainer ParseNodes(FBX::Document* document)
{
    AssetsContainer container;
    for (const auto& objects : document->SubNodes("Objects")) {
        for (const auto& model : objects->SubNodes("Model")) {
            auto node = Node::Create(model->Property(1));
            auto mesh = Mesh::Create();
            node->SetId(model->Property(0));
            node->SetComponent(mesh);
            mesh->SetId(model->Property(0));
            container.AddComponent(node);
        }
    }
    return container;
}

static inline AssetsContainer ParseMaterials(FBX::Document* document)
{
    AssetsContainer container;
    for (const auto& objects : document->SubNodes("Objects")) {
        if (objects == nullptr)
            continue;
        for (const auto& fbxMaterial : objects->SubNodes("Material")) {
            int64_t id(fbxMaterial->Property(0));
            std::string name(fbxMaterial->Property(1));
            auto material(Material::Create(name));
            container.AddComponent(material);
            material->SetId(id);
            auto P70(fbxMaterial->SubNode("Properties70"));
            if (P70 == nullptr)
                continue;
            std::map<std::string, std::shared_ptr<FBX::Node>> propertiesMap;
            for (const auto& P : P70->SubNodes("P")) {
                //Property is empty, ignore it.
                if (P->properties.size() < 2)
                    continue;
                std::string propertyName(P->Property(0));
                std::string propertyType(P->Property(1));
                std::cout << name << " " << propertyName << " " << propertyType << std::endl;
                propertiesMap[P->Property(0)] = P;
            }
            auto P(propertiesMap["DiffuseColor"]);
            material->SetAlbedo(P ? glm::vec3(
                                    double(P->Property(4)),
                                    double(P->Property(5)),
                                    double(P->Property(6)))
                                  : glm::vec3(0.5));
            P = propertiesMap["EmissiveColor"];
            material->SetEmitting(P ? glm::vec3(
                                      double(P->Property(4)),
                                      double(P->Property(5)),
                                      double(P->Property(6)))
                                    : glm::vec3(0));
            P = propertiesMap["SpecularColor"] ? propertiesMap["SpecularColor"] : propertiesMap["ReflectionColor"];
            material->SetSpecular(P ? glm::vec3(
                                      double(P->Property(4)),
                                      double(P->Property(5)),
                                      double(P->Property(6)))
                                    : glm::vec3(0.04));
            P = propertiesMap["EmissiveFactor"];
            material->SetEmitting(material->Emitting() * float(P ? double(P->Property(4)) : 1.0));
            P = propertiesMap["SpecularFactor"];
            material->SetRoughness(P ? glm::clamp(1.f / (1.f + double(P->Property(4))) * 50.f, 0.0, 1.0) : 0.5);
            P = propertiesMap["ReflectionFactor"];
            material->SetMetallic(P ? double(P->Property(4)) : 0.0);
        }
    }
    return container;
}

AssetsContainer FBX::Parse(const std::filesystem::path path)
{
    AssetsContainer container;
    auto document(FBX::Document::Parse(path));
    document->Print();
    auto scene(Scene::Create(path.string()));
    container += ParseNodes(document);
    container += ParseMaterials(document);
    container += ParseGeometries(document);
    for (const auto& objects : document->SubNodes("Objects")) {
        if (objects == nullptr)
            continue;
        for (const auto& model : objects->SubNodes("Model")) {
            auto node(container.GetComponentByID<::Node>(model->Property(0)));
            if (node == nullptr) {
                std::cout << "WE DID NOT GET NODE" << int64_t(model->Property(0)) << "FOR SOME REASON" << std::endl;
                continue;
            }
            auto mesh = node->GetComponent<Mesh>();
            auto transform(node->GetComponent<Transform>());
            auto properties(model->SubNode("Properties70"));
            for (auto property : properties->SubNodes("P")) {
                std::string propertyName(property->Property(0));
                std::cout << propertyName << std::endl;
                if (propertyName == "Lcl Translation") {
                    transform->SetPosition(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << transform->Position().x << " " << transform->Position().y << " " << transform->Position().z << std::endl;
                } else if (propertyName == "Lcl Rotation") {
                    transform->SetRotation(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << transform->Rotation().x << " " << transform->Rotation().y << " " << transform->Rotation().z << std::endl;
                } else if (propertyName == "Lcl Scaling") {
                    transform->SetScale(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << transform->Scale().x << " " << transform->Scale().y << " " << transform->Scale().z << std::endl;
                }
                if (propertyName == "GeometricTranslation") {
                    if (mesh->GetComponent<Transform>() == nullptr)
                        mesh->SetComponent(Transform::Create());
                    mesh->GetComponent<Transform>()->SetPosition(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << mesh->GetComponent<Transform>()->Position().x << " " << mesh->GetComponent<Transform>()->Position().y << " " << mesh->GetComponent<Transform>()->Position().z << std::endl;
                } else if (propertyName == "GeometricRotation") {
                    if (mesh->GetComponent<Transform>() == nullptr)
                        mesh->SetComponent(Transform::Create());
                    mesh->GetComponent<Transform>()->SetRotation(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << mesh->GetComponent<Transform>()->Rotation().x << " " << mesh->GetComponent<Transform>()->Rotation().y << " " << mesh->GetComponent<Transform>()->Rotation().z << std::endl;
                } else if (propertyName == "GeometricScaling") {
                    if (mesh->GetComponent<Transform>() == nullptr)
                        mesh->SetComponent(Transform::Create());
                    mesh->GetComponent<Transform>()->SetScale(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << mesh->GetComponent<Transform>()->Scale().x << " " << mesh->GetComponent<Transform>()->Scale().y << " " << mesh->GetComponent<Transform>()->Scale().z << std::endl;
                }
            }
        }
    }
    std::cout << "Parsing done, printing Container\n";
    for (const auto& components : container.GetComponentsInChildren()) {
        std::cout << "Type : " << components.first.name() << '\n';
        for (const auto& component : components.second) {
            std::cout << component->Id() << ' ' << component->Name() << '\n';
        }
    }

    std::cout << "Setting up relations" << std::endl;
    for (const auto& connection : document->SubNodes("Connections")) {
        for (const auto& c : connection->SubNodes("C")) {
            int64_t sourceId(c->Property(1));
            int64_t destinationId(c->Property(2));
            if (std::string(c->Property(0)) == "OO") {
                {
                    auto source = container.GetComponentByID<::Node>(sourceId);
                    if (source != nullptr) {
                        std::cout << "Got Node " << source->Id() << std::endl;
                        {
                            auto destination = container.GetComponentByID<::Node>(destinationId);
                            if (destination != nullptr) {
                                std::cout << source->Id() << " : IS CHILD OF : " << destination->Id() << std::endl;
                                source->SetParent(destination);
                            } else if (destinationId == 0) {
                                std::cout << source->Id() << " : IS CHILD OF : "
                                          << "*ROOT*" << std::endl;
                                scene->Add(source);
                            }
                        }
                        continue;
                    }
                }
                {
                    auto source = container.GetComponentsByID<::Geometry>(sourceId);
                    if (!source.empty()) {
                        std::cout << "Got Geometry " << sourceId << std::endl;
                        {
                            auto destination = container.GetComponentByID<::Node>(destinationId);
                            if (destination != nullptr) {
                                std::cout << "Mesh " << destinationId << " uses Geometry " << sourceId << std::endl;
                                for (auto vg : source)
                                    destination->GetComponent<Mesh>()->AddGeometry(vg);
                            }
                            continue;
                        }
                    }
                }
                {
                    auto source = container.GetComponentByID<::Material>(sourceId);
                    if (source != nullptr) {
                        std::cout << "Got Material " << source->Id() << std::endl;
                        {
                            auto destination = container.GetComponentByID<::Node>(destinationId);
                            if (destination != nullptr) {
                                std::cout << "Mesh " << destinationId << " uses Material " << source->Id() << std::endl;
                                destination->GetComponent<Mesh>()->AddMaterial(source);
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
    delete document;
    container.AddComponent(scene);
    return container;
}