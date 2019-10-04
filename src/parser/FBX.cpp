/*
 * @Author: gpi
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpi
 * @Last Modified time: 2019-10-04 16:15:36
 */

#include "parser/FBX.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "MeshParser.hpp"
#include "Vgroup.hpp"
#include "parser/FBX/FBXDocument.hpp"
#include "parser/FBX/FBXNode.hpp"
#include "parser/FBX/FBXObject.hpp"
#include "parser/FBX/FBXProperty.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

//Add this parser to MeshParser !
auto __fbxParser = MeshParser::Add("fbx", FBX::parseMesh);

static inline std::vector<glm::vec2> parseUV(FBX::Node *layerElementUV)
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
}

static inline auto getNormals(std::shared_ptr<FBX::Node> layerElementNormal)
{
    std::vector<CVEC4> vn;
    if (layerElementNormal == nullptr)
        return vn;
    auto normals(layerElementNormal->SubNode("Normals"));

    FBX::Array vnArray(normals->Property(0));
    for (auto i = 0u; i < vnArray.length / 3; i++)
    {
        glm::vec3 nd(
            std::get<double *>(vnArray.data)[i * 3 + 0],
            std::get<double *>(vnArray.data)[i * 3 + 1],
            std::get<double *>(vnArray.data)[i * 3 + 2]);
        nd = (nd + 1.f) * 0.5f * 255.f;
        CVEC4 n(nd, 255);
        vn.push_back(n);
    }
    auto referenceInformationType(layerElementNormal->SubNode("ReferenceInformationType"));
    std::string referenceType(referenceInformationType ? std::string(referenceInformationType->Property(0)) : std::string(""));
    if (referenceType == "" || referenceType == "Direct")
        return vn;
    auto normalsIndex(layerElementNormal->SubNode("NormalsIndex"));
    if (normalsIndex == nullptr)
        throw std::runtime_error("Node(" + layerElementNormal->Name() + ") missing NormalsIndex.");
    auto normalsIndexArray(FBX::Array(normalsIndex->Property(0)));
    std::vector<CVEC4> vnUnindexed;
    vnUnindexed.reserve(vn.size());
    for (auto i = 0u; i < normalsIndexArray.length; i++)
    {
        auto index = ((int32_t *)normalsIndexArray)[i];
        vnUnindexed.push_back(vn.at(index));
    }
    return vnUnindexed;
}

static inline auto getVertices(std::shared_ptr<FBX::Node> vertices)
{
    std::vector<glm::vec3> v;
    if (vertices == nullptr)
        return v;
    FBX::Array vArray(vertices->Property(0));
    for (auto i = 0u; i < vArray.length / 3; i++)
    {
        auto vec3 = glm::vec3(
            std::get<double *>(vArray.data)[i * 3 + 0],
            std::get<double *>(vArray.data)[i * 3 + 1],
            std::get<double *>(vArray.data)[i * 3 + 2]);
        v.push_back(vec3);
    }
    return v;
}

static inline auto getVerticesIndices(std::shared_ptr<FBX::Node> polygonVertexIndex)
{
    std::vector<int32_t> vi;
    if (polygonVertexIndex == nullptr)
        return vi;
    FBX::Array viArray(polygonVertexIndex->Property(0));
    for (auto i = 0u; i < viArray.length; i++)
    {
        vi.push_back(std::get<int32_t *>(viArray.data)[i]);
    }
    return vi;
}

static inline auto triangulateIndices(const std::vector<int32_t> &vi)
{
    std::vector<unsigned> indices;
    std::vector<int32_t> polygonIndex;
    for (const auto i : vi)
    {
        if (i < 0)
        {
            polygonIndex.push_back(abs(i) - 1);
            indices.push_back(polygonIndex.at(0));
            indices.push_back(polygonIndex.at(1));
            indices.push_back(polygonIndex.at(2));
            if (polygonIndex.size() == 4)
            {
                indices.push_back(polygonIndex.at(2));
                indices.push_back(polygonIndex.at(3));
                indices.push_back(polygonIndex.at(0));
            }
            polygonIndex.clear();
        }
        else
        {
            polygonIndex.push_back(i);
        }
    }
    return indices;
}

static inline auto getMappedIndices(const std::string &mappingInformationType, const std::vector<int32_t> &polygonIndices)
{
    std::vector<unsigned> normalsIndices;
    if (mappingInformationType == "ByPolygonVertex")
    {
        normalsIndices.resize(polygonIndices.size());
        for (auto i = 0u; i < polygonIndices.size(); i++)
        {
            normalsIndices.at(i) = i;
        }
    }
    else if (mappingInformationType == "ByVertex" || mappingInformationType == "ByVertice")
    {
        normalsIndices.resize(polygonIndices.size());
        std::cout << "Generate Normal Indice" << std::endl;
        for (auto i = 0u; i < polygonIndices.size(); i++)
        {
            auto index = polygonIndices.at(i);
            index = index >= 0 ? index : abs(index) - 1;
            normalsIndices.at(i) = index;
            //normalsIndices.at(index) = i / 3;
        }
    }
    else if (mappingInformationType == "ByPolygon")
    {
        int normalIndex = 0;
        normalsIndices.resize(polygonIndices.size());
        for (auto i = 0u; i < polygonIndices.size(); i++)
        {
            auto index = polygonIndices.at(i);
            normalsIndices.at(i) = normalIndex;
            if (index < 0)
            {
                normalIndex++;
            }
        }
    }
    std::cout << "Generate Normal Indice DONE" << std::endl;
    return normalsIndices;
}

std::shared_ptr<Mesh> FBX::parseMesh(const std::string &name, const std::string &path)
{
    auto document(FBX::Document::Parse(path));
    document->Print();
    auto mainMesh(Mesh::Create(name));
    auto mtl = Material::Create("default_fbx");
    mtl->albedo = glm::vec3(0.5);
    mtl->roughness = 0.5;
    for (const auto &objects : document->SubNodes("Objects"))
    {
        if (objects == nullptr)
            continue;
        for (const auto &fbxMaterial : objects->SubNodes("Material"))
        {
            int64_t id(fbxMaterial->Property(0));
            std::string name(fbxMaterial->Property(1));
            auto material(Material::Create(name));
            material->SetId(id);
            for (const auto &P70 : fbxMaterial->SubNodes("Properties70"))
            {
                //Properties70 is empty, ignore it.
                if (P70->properties.size() < 2)
                    continue;
                std::string propertyName(P70->Property(0));
                std::string propertyType(P70->Property(1));
                if ("DiffuseColor" == propertyName)
                {
                    if ("Color" == propertyType)
                        material->albedo = glm::vec3(
                            double(P70->Property(4)),
                            double(P70->Property(5)),
                            double(P70->Property(6)));
                }
                else if ("EmissiveColor" == propertyName)
                {
                    if ("Color" == propertyType)
                        material->emitting = glm::vec3(
                            double(P70->Property(4)),
                            double(P70->Property(5)),
                            double(P70->Property(6)));
                }
                else if ("AmbientColor" == propertyName)
                {
                    if ("Color" == propertyType)
                        continue;
                }
                else if ("SpecularColor" == propertyName)
                {
                    if ("Color" == propertyType)
                        material->specular = glm::vec3(
                            double(P70->Property(4)),
                            double(P70->Property(5)),
                            double(P70->Property(6)));
                }
                else if ("ReflectionColor" == propertyName)
                {
                    if ("Color" == propertyType)
                        material->specular = glm::vec3(
                            double(P70->Property(4)),
                            double(P70->Property(5)),
                            double(P70->Property(6)));
                }
                else if ("EmissiveFactor" == propertyName)
                {
                    if ("Number" == propertyType)
                        material->emitting *= double(P70->Property(4));
                    continue;
                }
                else if ("AmbientFactor" == propertyName)
                {
                    if ("Number" == propertyType)
                        continue;
                }
                else if ("SpecularFactor" == propertyName)
                {
                    if ("Number" == propertyType)
                        material->roughness = glm::clamp(1.f / (1.f + double(P70->Property(4))) * 50.f, 0.0, 1.0);
                }
                else if ("Shininess" == propertyName)
                {
                    if ("Number" == propertyType)
                        continue;
                }
                else if ("ShininessExponent" == propertyName)
                {
                    if ("Number" == propertyType)
                        continue;
                }
                else if ("ReflectionFactor" == propertyName)
                {
                    if ("Number" == propertyType)
                        material->metallic = double(P70->Property(4));
                }
            }
        }
        for (const auto &geometry : objects->SubNodes("Geometry"))
        {
            if (geometry == nullptr)
                continue;
            auto vgroup(Vgroup::Create(""));
            vgroup->SetId(int64_t(geometry->Property(0))); //first property is Geometry ID
            std::cout << "Geometry ID " << int64_t(geometry->Property(0)) << std::endl;
            auto vertices(getVertices(geometry->SubNode("Vertices")));
            if (vertices.empty())
                continue;
            auto verticesIndices(getVerticesIndices(geometry->SubNode("PolygonVertexIndex")));
            auto layerElementNormal(geometry->SubNode("LayerElementNormal"));
            auto normals = getNormals(layerElementNormal);
            std::vector<unsigned> normalsIndices;
            if (layerElementNormal != nullptr)
                normalsIndices = getMappedIndices(layerElementNormal->SubNode("MappingInformationType")->Property(0), verticesIndices);
            auto layerElementMaterial(geometry->SubNode("LayerElementMaterial"));
            auto material(-1);
            for (auto &materialIndex : getVerticesIndices(layerElementMaterial->SubNode("Materials")))
            {
                if (material != materialIndex)
                {
                    std::cout << materialIndex << " ";
                    material = materialIndex;
                }
            }
            std::cout << std::endl;
            std::vector<int32_t> polygonIndex;
            std::vector<int32_t> polygonIndexNormal;
            for (auto i = 0u; i < verticesIndices.size(); i++)
            {
                auto index = verticesIndices.at(i);
                polygonIndex.push_back(index >= 0 ? index : abs(index) - 1);
                polygonIndexNormal.push_back(normalsIndices.at(i));
                if (index < 0)
                {
                    vgroup->v.push_back(vertices.at(polygonIndex.at(0)));
                    vgroup->vn.push_back(normals.at(polygonIndexNormal.at(0)));
                    vgroup->v.push_back(vertices.at(polygonIndex.at(1)));
                    vgroup->vn.push_back(normals.at(polygonIndexNormal.at(1)));
                    vgroup->v.push_back(vertices.at(polygonIndex.at(2)));
                    vgroup->vn.push_back(normals.at(polygonIndexNormal.at(2)));
                    if (polygonIndex.size() == 4)
                    {
                        vgroup->v.push_back(vertices.at(polygonIndex.at(2)));
                        vgroup->vn.push_back(normals.at(polygonIndexNormal.at(2)));
                        vgroup->v.push_back(vertices.at(polygonIndex.at(3)));
                        vgroup->vn.push_back(normals.at(polygonIndexNormal.at(3)));
                        vgroup->v.push_back(vertices.at(polygonIndex.at(0)));
                        vgroup->vn.push_back(normals.at(polygonIndexNormal.at(0)));
                    }
                    polygonIndex.clear();
                    polygonIndexNormal.clear();
                    continue;
                }
            }
            vgroup->set_material(mtl);
        }
        for (const auto &model : objects->SubNodes("Model"))
        {
            auto mesh(Mesh::GetById(model->Property(0)));
            if (mesh == nullptr)
            {
                mesh = Mesh::Create(model->Property(1));
                mesh->SetId(model->Property(0));
            }
            auto properties(model->SubNode("Properties70"));
            for (auto property : properties->SubNodes("P"))
            {
                std::string propertyName(property->Property(0));
                std::cout << propertyName << std::endl;
                if (propertyName == "Lcl Translation")
                {
                    mesh->SetPosition(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << mesh->Position().x << " " << mesh->Position().y << " " << mesh->Position().z << std::endl;
                }
                else if (propertyName == "Lcl Rotation")
                {
                    mesh->SetRotation(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << mesh->Rotation().x << " " << mesh->Rotation().y << " " << mesh->Rotation().z << std::endl;
                }
                else if (propertyName == "Lcl Scaling")
                {
                    mesh->SetScale(glm::vec3(
                        double(property->Property(4)),
                        double(property->Property(5)),
                        double(property->Property(6))));
                    std::cout << mesh->Scale().x << " " << mesh->Scale().y << " " << mesh->Scale().z << std::endl;
                }
            }
        }
    }
    std::cout << "Setting up relations" << std::endl;
    for (const auto &connection : document->SubNodes("Connections"))
    {
        for (const auto &c : connection->SubNodes("C"))
        {
            int64_t sourceId(c->Property(1));
            int64_t destinationId(c->Property(2));
            if (std::string(c->Property(0)) == "OO")
            {
                {
                    auto source(Mesh::GetById(sourceId));
                    if (source != nullptr)
                    {
                        std::cout << "Got Mesh " << source->Id() << std::endl;
                        {
                            auto destination(Mesh::GetById(destinationId));
                            if (destination != nullptr)
                            {
                                std::cout << source->Id() << " : IS CHILD OF : " << destination->Id() << std::endl;
                                destination->add_child(source);
                            }
                            else if (destinationId == 0)
                            {
                                std::cout << source->Id() << " : IS CHILD OF : "
                                          << "*ROOT*" << std::endl;
                                mainMesh->add_child(source);
                            }
                        }
                        continue;
                    }
                }
                {
                    auto source(Vgroup::GetById(sourceId));
                    if (source != nullptr)
                    {
                        std::cout << "Got Vgroup " << source->Id() << std::endl;
                        {
                            auto destination(Mesh::GetById(destinationId));
                            if (destination != nullptr)
                            {
                                std::cout << "Mesh " << destinationId << " uses Vgroup " << source->Id() << std::endl;
                                destination->AddVgroup(source);
                            }
                            continue;
                        }
                    }
                }
                {
                    auto source(Material::GetById(sourceId));
                    if (source != nullptr)
                    {
                        std::cout << "Got Material " << source->Id() << std::endl;
                        std::cout << "Destination ID " << destinationId << std::endl;
                        {
                            auto destination(Vgroup::GetById(destinationId));
                            if (destination != nullptr)
                            {
                                std::cout << "Vgroup " << destinationId << " uses Material " << source->Id() << std::endl;
                                destination->set_material(source);
                                continue;
                            }
                        }
                        {
                            auto destination(Mesh::GetById(destinationId));
                            if (destination != nullptr)
                            {
                                std::cout << "Mesh " << destinationId << " uses Material " << source->Id() << std::endl;
                                for (auto vg : destination->vgroups())
                                    vg->set_material(source);
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
    return mainMesh;
}
