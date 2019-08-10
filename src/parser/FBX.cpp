/*
 * @Author: gpi
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2019-08-10 12:39:48
 */

#include "parser/FBX.hpp"
#include "MeshParser.hpp"
#include "parser/FBX/FBXDocument.hpp"
#include "parser/FBX/FBXNode.hpp"
#include "parser/FBX/FBXProperty.hpp"
#include <iostream>

//Add this parser to MeshParser !
auto __fbxParser = MeshParser::add("fbx", FBX::parseMesh);

#include "Material.hpp"
#include "Mesh.hpp"
#include "Vgroup.hpp"
#include <glm/glm.hpp>

static inline std::vector<glm::vec2> parseUV(FBX::Node* layerElementUV)
{
    std::vector<glm::vec2> uv;
    if (layerElementUV == nullptr)
        return uv;
    layerElementUV->SubNode("UV")->Print();
    auto UV = layerElementUV->SubNode("UV");
    if (UV == nullptr)
        return uv;
    FBX::Array UVArray(UV->Property(0));
    for (auto i = 0u; i < UVArray.length / 2; i++) {
        auto vec2 = glm::vec2(
            std::get<double*>(UVArray.data)[i * 2 + 0],
            std::get<double*>(UVArray.data)[i * 2 + 1]);
        uv.push_back(vec2);
    }
    auto UVIndex = layerElementUV->SubNode("UVIndex");
    if (UVIndex != nullptr) {
        std::vector<glm::vec2> realUV;
        FBX::Array UVIndexArray(UVIndex->Property(0));
        for (auto i = 0u; i < UVIndexArray.length; i++) {
            auto index = std::get<int32_t*>(UVIndexArray.data)[i];
            realUV.push_back(uv.at(index));
        }
        return realUV;
    }
    return uv;
}

static inline auto parseNormals(FBX::Node* layerElementNormal)
{
    std::vector<CVEC4> vn;
    if (layerElementNormal == nullptr)
        return vn;
    std::cout << std::string(layerElementNormal->SubNode("MappingInformationType")->Property(0)) << std::endl;
    auto normals(layerElementNormal->SubNode("Normals"));
    FBX::Array vnArray(normals->Property(0));
    std::cout << normals->Property(0).typeCode << std::endl;
    std::cout << normals->Property(0).data.index() << std::endl;
    std::cout << vnArray.data.index() << std::endl;
    for (auto i = 0u; i < vnArray.length / 3; i++) {
        CVEC4 n;
        n.x = std::get<double*>(vnArray.data)[i * 3 + 0];
        n.y = std::get<double*>(vnArray.data)[i * 3 + 1];
        n.z = std::get<double*>(vnArray.data)[i * 3 + 2];
        n.w = 255;
        vn.push_back(n);
    }
    //layerElementNormal->nodes["MappingInformationType"]
    return vn;
}

static inline auto parseVertices(FBX::Node* vertices, FBX::Node* polygonVertexIndex)
{
    std::vector<glm::vec3> v;
    if (vertices == nullptr)
        return v;
    FBX::Array vArray(vertices->Property(0));
    for (auto i = 0u; i < vArray.length; i++) {
        auto vec3 = glm::vec3(
            std::get<double*>(vArray.data)[i * 3 + 0],
            std::get<double*>(vArray.data)[i * 3 + 1],
            std::get<double*>(vArray.data)[i * 3 + 2]);
        v.push_back(vec3);
    }
    if (polygonVertexIndex == nullptr)
        return v;
    std::vector<int32_t> vi;
    FBX::Array viArray(polygonVertexIndex->Property(0));
    std::cout << "viArray " << viArray.data.index() << std::endl;
    for (auto i = 0u; i < viArray.length; i++) {
        vi.push_back(std::get<int32_t*>(viArray.data)[i]);
    }
    std::vector<glm::vec3> realV;
    std::vector<int32_t> polygonIndex;
    for (const auto i : vi) {
        if (i < 0) {
            polygonIndex.push_back(abs(i) - 1);
            realV.push_back(v.at(polygonIndex.at(0)));
            realV.push_back(v.at(polygonIndex.at(1)));
            realV.push_back(v.at(polygonIndex.at(2)));
            if (polygonIndex.size() == 4) {
                realV.push_back(v.at(polygonIndex.at(2)));
                realV.push_back(v.at(polygonIndex.at(3)));
                realV.push_back(v.at(polygonIndex.at(0)));
            }
            polygonIndex.clear();
        } else {
            polygonIndex.push_back(i);
        }
    }
    std::cout << "Parsing Done ! " << realV.size() << std::endl;
    return realV;
}

std::shared_ptr<Mesh> FBX::parseMesh(const std::string& name, const std::string& path)
{
    auto document = FBX::Document::Parse(path);
    //document.Print();
    auto mesh(Mesh::create(name));
    auto mtl = Material::create("default_fbx");
    mtl->albedo = glm::vec3(0.5);
    mtl->roughness = 0.5;
    for (const auto& objects : document->SubNodes("Objects")) {
        if (objects == nullptr)
            continue;
        for (const auto& geometry : objects->SubNodes("Geometry")) {
            if (geometry == nullptr)
                continue;
            auto geometryId(std::to_string(int64_t(geometry->Property(0))));
            auto vgroup(Vgroup::create(geometryId)); //first property is Geometry ID

            vgroup->vt = parseUV(geometry->SubNode("layerElementUV"));
            vgroup->vn = parseNormals(geometry->SubNode("LayerElementNormal"));
            vgroup->v = parseVertices(geometry->SubNode("Vertices"),
                geometry->SubNode("PolygonVertexIndex"));
            vgroup->set_material(mtl);
            auto meshChild(Mesh::create(geometryId));
            meshChild->add(vgroup);
            mesh->add_child(meshChild);
            //mesh->add(vgroup);
        }
    }
    return mesh;
}
