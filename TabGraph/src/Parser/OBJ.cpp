/*
* @Author: gpinchon
* @Date:   2020-08-17 14:43:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 21:09:26
*/
/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-09 17:11:29
*/

#include "Parser/OBJ.hpp"
#include "Assets/AssetsParser.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Engine.hpp" // for M_PI
#include "Material/Material.hpp" // for Material
#include "Mesh/Geometry.hpp" // for Geometry, CVEC4
#include "Mesh/Mesh.hpp" // for Mesh
#include "Parser/InternalTools.hpp" // for count_char, split_...
#include "Parser/MTLLIB.hpp" // for parse
#include "Physics/BoundingAABB.hpp" // for BoundingAABB
#include "Physics/BoundingElement.hpp" // for BoundingElement
#include "Scene/Scene.hpp"
#include "Tools.hpp"
#include <algorithm> // for max, min
//#include <bits/exception.h> // for exception
#include <errno.h> // for errno
//#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <filesystem>
#include <glm/glm.hpp> // for s_vec3, s_vec2, glm::vec2, glm::vec3
#include <math.h> // for atan2
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fgets, fopen
#include <string.h> // for memset, strerror
#include <vector> // for vector

#ifdef _WIN32
#include <io.h>
#ifndef R_OK
#define R_OK 4
#endif
#else
#include <sys/io.h>
#endif // for access, R_OK

auto __objParser = AssetsParser::Add(".obj", OBJ::Parse);

struct ObjContainer {
    AssetsContainer container;
    std::shared_ptr<Geometry> currentGeometry;
    std::vector<glm::vec3> v;
    std::vector<glm::vec3> vn;
    std::vector<glm::vec2> vt;
    std::filesystem::path path;
};

static int get_vi(const std::vector<glm::vec2>& v, const std::string& str)
{
    int vindex;

    vindex = std::stoi(str);
    if (vindex < 0) {
        vindex = v.size() + vindex;
    } else {
        vindex -= 1;
    }
    if (vindex < 0 || static_cast<unsigned>(vindex) >= v.size()) {
        return (-1);
    }
    return (vindex);
}

static int get_vi(const std::vector<glm::vec3>& v, const std::string& str)
{
    int vindex;

    vindex = std::stoi(str);
    if (vindex < 0) {
        vindex = v.size() + vindex;
    } else {
        vindex -= 1;
    }
    if (vindex < 0 || static_cast<unsigned>(vindex) >= v.size()) {
        return (-1);
    }
    return (vindex);
}

static void parse_indice(ObjContainer& p, std::vector<std::string>& split, int vindex[3][3])
{
    for (auto i = 0; i < 3; i++) {
        vindex[i][0] = -1;
        vindex[i][1] = -1;
        vindex[i][2] = -1;
    }
    for (auto i = 0u; i < split.size() && i < 3; i++) {
        //while (i < split.size() && i < 3) {
        auto fsplit = strsplit(split[i], '/');
        auto splitLen = fsplit.size();
        auto slashCount = count_char(split[i], '/');
        vindex[0][i] = -1;
        vindex[1][i] = -1;
        vindex[2][i] = -1;
        vindex[0][i] = get_vi(p.v, fsplit[0]);
        if (vindex[0][i] == -1) {
            return;
        }
        if ((splitLen == 3 && slashCount == 2) || (splitLen == 2 && slashCount == 1)) {
            vindex[2][i] = get_vi(p.vt, fsplit[1]);
        }
        if (splitLen == 3 && slashCount == 2) {
            vindex[1][i] = get_vi(p.vn, fsplit[2]);
        } else if (splitLen == 2 && slashCount == 2) {
            vindex[1][i] = get_vi(p.vn, fsplit[1]);
        }
        //i++;
    }
}

static void parse_vn(ObjContainer& p, int vindex[3][3], glm::vec3 v[3], glm::vec3 vn[3])
{
    short i;

    i = 0;
    while (i < 3) {
        if (vindex[1][i] != -1) {
            vn[i] = p.vn[vindex[1][i]];
        } else {
            vn[i] = generate_vn(v);
        }
        i++;
    }
}

static void push_values(ObjContainer& p, glm::vec3* v, glm::vec3* vn, glm::vec2* vt)
{
    if (p.currentGeometry->Accessor(Geometry::AccessorKey::Position) == nullptr)
        p.currentGeometry->SetAccessor(Geometry::AccessorKey::Position, BufferHelper::CreateAccessor<glm::vec3>(0));
    if (p.currentGeometry->Accessor(Geometry::AccessorKey::Normal) == nullptr)
        p.currentGeometry->SetAccessor(Geometry::AccessorKey::Normal, BufferHelper::CreateAccessor<glm::vec3>(0));
    if (p.currentGeometry->Accessor(Geometry::AccessorKey::TexCoord_0) == nullptr)
        p.currentGeometry->SetAccessor(Geometry::AccessorKey::TexCoord_0, BufferHelper::CreateAccessor<glm::vec2>(0));
    for (auto index(0u); index < 3; ++index) {
        BufferHelper::PushBack(p.currentGeometry->Accessor(Geometry::AccessorKey::Position), v[index]);
        BufferHelper::PushBack(p.currentGeometry->Accessor(Geometry::AccessorKey::Normal), vn[index]);
        BufferHelper::PushBack(p.currentGeometry->Accessor(Geometry::AccessorKey::TexCoord_0), vt[index]);
    }
}

void parse_v(ObjContainer& p, std::vector<std::string>& split, glm::vec2* in_vt)
{
    int vindex[3][3];
    glm::vec3 v[3];
    glm::vec3 vn[3];
    glm::vec2 vt[3];
    short i;

    parse_indice(p, split, vindex);
    i = 0;
    while (i < 3) {
        if (vindex[0][i] == -1) {
            return;
        }
        v[i] = p.v[vindex[0][i]];
        if (vindex[2][i] != -1) {
            vt[i] = p.vt[vindex[2][i]];
            in_vt = (glm::vec2*)0x1;
        } else {
            vt[i] = in_vt != nullptr ? in_vt[i] : generate_vt(v[i], glm::vec3());
        }
        i++;
    }
    parse_vn(p, vindex, v, vn);
    if (in_vt == nullptr) {
        correct_vt(vt);
    }
    push_values(p, v, vn, vt);
}

void parse_vg(ObjContainer& p, const std::string& name = "")
{
    static int childNbr = 0;
    childNbr++;

    if (name == "") {
        p.currentGeometry = Geometry::Create(p.container.GetComponent<Mesh>()->Name() + "_Geometry_" + std::to_string(childNbr));
    } else {
        p.currentGeometry = Geometry::Create(name);
    }
    p.container.GetComponent<Mesh>()->AddGeometry(p.currentGeometry);
}

void correct_vt(glm::vec2* vt)
{
    glm::vec3 v[3];
    glm::vec3 texnormal { 0, 0, 0 };

    v[0] = glm::vec3(vt[0], 0.f);
    v[1] = glm::vec3(vt[1], 0.f);
    v[2] = glm::vec3(vt[2], 0.f);
    texnormal = glm::cross(v[1] - v[0], v[2] - v[0]);
    if (texnormal.z > 0) {
        if (vt[0].x < 0.25f) {
            vt[0].x += 1.f;
        }
        if (vt[1].x < 0.25f) {
            vt[1].x += 1.f;
        }
        if (vt[2].x < 0.25f) {
            vt[2].x += 1.f;
        }
    }
}

glm::vec2 generate_vt(glm::vec3 v, glm::vec3 center)
{
    glm::vec2 vt { 0, 0 };
    glm::vec3 vec { 0, 0, 0 };

    vec = glm::normalize(center - v);
    vt.x = 0.5f + (atan2(vec.z, vec.x) / (2 * M_PI));
    vt.y = 0.5f + -vec.y * 0.5f;
    return (vt);
}

glm::vec3 generate_vn(glm::vec3* v)
{
    return (glm::normalize(glm::cross(v[1] - v[0], v[2] - v[0])));
}

glm::vec3 parse_vec3(std::vector<std::string>& split)
{
    float v[3];
    unsigned i;

    i = 0;
    memset(v, 0, sizeof(float) * 3);
    while (i < 3) {
        if ((i + 1) >= split.size()) {
            break;
        }
        v[i] = std::stof(split[i + 1]);
        i++;
    }
    return (glm::vec3(v[0], v[1], v[2]));
}

glm::vec2 parse_vec2(std::vector<std::string>& split)
{
    float v[2];
    unsigned i;

    i = 0;
    memset(v, 0, sizeof(float) * 2);
    while (i < 2) {
        if ((i + 1) >= split.size()) {
            break;
        }
        v[i] = std::stof(split[i + 1]);
        i++;
    }
    return (glm::vec2(v[0], v[1]));
}

void parse_vtn(ObjContainer& p, std::vector<std::string>& split)
{
    glm::vec3 v { 0, 0, 0 };
    glm::vec2 vt { 0, 0 };

    if (split[0] == "v") {
        v = parse_vec3(split);
        p.v.push_back(v);
    } else if (split[0] == "vn") {
        v = parse_vec3(split);
        p.vn.push_back(v);
    } else if (split[0] == "vt") {
        vt = parse_vec2(split);
        vt.y = vt.y;
        p.vt.push_back(vt);
    }
}

static void parse_f(ObjContainer& p, std::vector<std::string>& split)
{
    short faces;
    short i;

    split.erase(split.begin());
    faces = split.size() - 3 + 1;
    i = 0;
    while (i < faces) {
        if (faces == 2 && i == 0) {
            auto lesplit = std::vector<std::string>({ split[0], split[i + 1], split[i + 2] });
            auto levector = std::vector<glm::vec2>({ glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1) });
            parse_v(p, lesplit, &levector[0]);
        } else if (faces == 2 && i >= 1) {
            auto lesplit = std::vector<std::string>({ split[0], split[i + 1], split[i + 2] });
            auto levector = std::vector<glm::vec2>({ glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0) });
            parse_v(p, lesplit, &levector[0]);
        } else {
            auto lesplit = std::vector<std::string>({ split[0], split[i + 1], split[i + 2] });
            parse_v(p, lesplit, nullptr);
        }
        i++;
    }
}

#include <Debug.hpp>

static void parse_line(ObjContainer& p, const char* line)
{
    auto split = strsplitwspace(line);
    if (split.empty() || split[0][0] == '#') {
        return;
    }
    if (split[0][0] == 'v') {
        parse_vtn(p, split);
    } else if (split[0][0] == 'f') {
        parse_f(p, split);
    } else if (split[0][0] == 'g' || split[0][0] == 'o') {
        auto mtlIndex(-1);
        if (p.currentGeometry != nullptr)
            mtlIndex = p.currentGeometry->MaterialIndex();
        parse_vg(p, split[1]);
        p.currentGeometry->SetMaterialIndex(mtlIndex);
    } else if (split[0] == "usemtl") {
        if (p.currentGeometry == nullptr || p.currentGeometry->Accessor(Geometry::AccessorKey::Position))
            parse_vg(p);
        auto mtl = p.container.GetComponentByName<Material>(split.at(1));
        auto mtlIndex(p.container.GetComponent<Mesh>()->GetMaterialIndex(mtl));
        if (mtlIndex == -1) {
            p.container.GetComponent<Mesh>()->AddMaterial(mtl);
            mtlIndex = p.container.GetComponent<Mesh>()->GetMaterialIndex(mtl);
        }
        p.currentGeometry->SetMaterialIndex(mtlIndex);
    } else if (split[0] == "mtllib") {
        p.container += AssetsParser::Parse((p.path.parent_path() / split[1]).string());
    }
}

static void start_obj_parsing(ObjContainer& p, const std::string& path)
{
    char line[4096];

    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    auto fd(fopen(path.c_str(), "r"));
    if (fd == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    p.container.AddComponent(Mesh::Create(path));
    auto l = 1;
    while (fgets(line, 4096, fd) != nullptr) {
        try {
            parse_line(p, line);
        } catch (std::exception& e) {
            throw std::runtime_error("Error at line : " + std::to_string(l) + " " + e.what());
        }
        l++;
    }
    fclose(fd);
    /*
    if (!p.currentGeometry->v.empty() != 0u)
    {
        parse_vg(p);
    }
    else
    {
        throw std::runtime_error(std::string("Invalid OBJ"));
    }
    */
}

AssetsContainer OBJ::Parse(const std::filesystem::path path)
{
    ObjContainer p;
    AssetsContainer container;

    p.path = path;
    try {
        start_obj_parsing(p, path.string());
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + path.string() + " :\n" + e.what());
    }
    container += p.container;
    auto scene(Scene::Create(path.string()));
    auto node(Node::Create(path.string() + "_node"));
    for (const auto& mesh : container.GetComponents<Mesh>()) {
        node->AddComponent(mesh);
    }
    scene->AddRootNode(node);
    container.AddComponent(scene);
    return (container);
}
