/*
* @Author: gpinchon
* @Date:   2020-08-17 14:43:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-10 22:09:47
*/
/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-09 17:11:29
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/BinaryData.hpp>
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Node.hpp>
#include <Parser/InternalTools.hpp>
#include <Physics/BoundingAABB.hpp>
#include <Physics/BoundingElement.hpp>
#include <Scene/Scene.hpp>
#include <Surface/Geometry.hpp>
#include <Surface/Mesh.hpp>
#include <Tools/Tools.hpp>
#include <Object.hpp>

#include <algorithm>
#include <errno.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <vector>

#ifdef _WIN32
#include <io.h>
#ifndef R_OK
#define R_OK 4
#endif
#else
#include <sys/io.h>
#endif // for access, R_OK

void ParseOBJ(std::shared_ptr<Assets::Asset>);

auto OBJMimeExtension {
    Assets::Parser::AddMimeExtension("model/obj", ".obj") //not standard but screw it.
};

auto OBJMimesParsers {
    Assets::Parser::Add("model/obj", ParseOBJ)
};
struct ObjContainer {
    std::shared_ptr<Assets::Asset> container { std::make_shared<Assets::Asset>() };
    std::shared_ptr<Geometry> currentGeometry { nullptr };
    std::shared_ptr<Buffer::View> currentBufferView { nullptr };
    std::shared_ptr<Assets::Asset> currentBuffer { nullptr };
    std::shared_ptr<BinaryData> binaryData { nullptr };
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

static auto parse_indice(ObjContainer& p, const std::vector<std::string>& split)
{
    std::array<std::array<int, 3>, 3> vindex{};
    for (auto i = 0; i < 3; i++) {
        vindex.at(i).at(0) = -1;
        vindex.at(i).at(1) = -1;
        vindex.at(i).at(2) = -1;
    }
    for (auto i = 0u; i < split.size() && i < 3; i++) {
        auto fsplit = strsplit(split.at(i), '/');
        auto splitLen = fsplit.size();
        auto slashCount = count_char(split[i], '/');
        vindex.at(0).at(i) = -1;
        vindex.at(1).at(i) = -1;
        vindex.at(2).at(i) = -1;
        vindex.at(0).at(i) = get_vi(p.v, fsplit[0]);
        if (vindex.at(0).at(i) == -1) {
            return vindex;
        }
        if ((splitLen == 3 && slashCount == 2) || (splitLen == 2 && slashCount == 1)) {
            vindex.at(2).at(i) = get_vi(p.vt, fsplit[1]);
        }
        if (splitLen == 3 && slashCount == 2) {
            vindex.at(1).at(i) = get_vi(p.vn, fsplit[2]);
        } else if (splitLen == 2 && slashCount == 2) {
            vindex.at(1).at(i) = get_vi(p.vn, fsplit[1]);
        }
    }
    return vindex;
}

static void parse_vn(ObjContainer& p, std::array<std::array<int, 3>, 3> vindex, glm::vec3 v[3], glm::vec3 vn[3])
{
    short i;

    i = 0;
    while (i < 3) {
        if (vindex.at(1).at(i) != -1) {
            vn[i] = p.vn[vindex.at(1).at(i)];
        } else {
            vn[i] = generate_vn(v);
        }
        i++;
    }
}

#define VERTEXBYTESIZE (sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2))

static void push_values(ObjContainer& p, glm::vec3* v, glm::vec3* vn, glm::vec2* vt)
{
    if (p.currentBuffer == nullptr) {
        p.binaryData = std::make_shared<BinaryData>(0);
        p.currentBuffer = std::make_shared<Assets::Asset>();
        p.currentBuffer->SetComponent(p.binaryData);
        p.currentBuffer->SetLoaded(true);
    }
    if (p.currentBufferView == nullptr) {
        p.currentBufferView = std::make_shared<Buffer::View>(p.currentBuffer, Buffer::View::Mode::Immutable);
        p.currentBufferView->SetType(Buffer::View::Type::Array);
        p.currentBufferView->SetByteStride(VERTEXBYTESIZE);
    }
    if (p.currentGeometry->Accessor(Geometry::AccessorKey::Position) == nullptr) {
        auto positionAccessor { std::make_shared<Buffer::Accessor>(Buffer::Accessor::ComponentType::Float32, Buffer::Accessor::Type::Vec3, p.currentBufferView) };
        positionAccessor->SetCount(0);
        positionAccessor->SetByteOffset(p.currentBufferView->GetByteLength());
        p.currentGeometry->SetAccessor(Geometry::AccessorKey::Position, positionAccessor);
    }
    if (p.currentGeometry->Accessor(Geometry::AccessorKey::Normal) == nullptr) {
        auto normalAccessor { std::make_shared<Buffer::Accessor>(Buffer::Accessor::ComponentType::Float32, Buffer::Accessor::Type::Vec3, p.currentBufferView) };
        normalAccessor->SetCount(0);
        normalAccessor->SetByteOffset(p.currentBufferView->GetByteLength() + sizeof(glm::vec3));
        normalAccessor->SetNormalized(true);
        p.currentGeometry->SetAccessor(Geometry::AccessorKey::Normal, normalAccessor);
    }
    if (p.currentGeometry->Accessor(Geometry::AccessorKey::TexCoord_0) == nullptr) {
        auto texcoordAccessor { std::make_shared<Buffer::Accessor>(Buffer::Accessor::ComponentType::Float32, Buffer::Accessor::Type::Vec2, p.currentBufferView) };
        texcoordAccessor->SetCount(0);
        texcoordAccessor->SetByteOffset(p.currentBufferView->GetByteLength() + sizeof(glm::vec3) + sizeof(glm::vec3));
        p.currentGeometry->SetAccessor(Geometry::AccessorKey::TexCoord_0, texcoordAccessor);
    }
    for (auto index(0u); index < 3; ++index) {
        p.binaryData->PushBack(v[index]);
        p.binaryData->PushBack(vn[index]);
        p.binaryData->PushBack(vt[index]);
    }
    p.currentBufferView->SetByteLength(p.binaryData->GetByteLength());
    auto vertexCount { p.currentGeometry->Accessor(Geometry::AccessorKey::Position)->GetCount() + 3 };
    p.currentGeometry->Accessor(Geometry::AccessorKey::Position)->SetCount(vertexCount);
    p.currentGeometry->Accessor(Geometry::AccessorKey::Normal)->SetCount(vertexCount);
    p.currentGeometry->Accessor(Geometry::AccessorKey::TexCoord_0)->SetCount(vertexCount);
}

void parse_v(ObjContainer& p, const std::vector<std::string>& split, std::vector<glm::vec2> in_vt)
{
    glm::vec3 v[3]{};
    glm::vec3 vn[3]{};
    glm::vec2 vt[3]{};
    short i;

    auto vindex { parse_indice(p, split) };
    i = 0;
    while (i < 3) {
        if (vindex[0][i] == -1) {
            return;
        }
        v[i] = p.v[vindex[0][i]];
        if (vindex[2][i] != -1) {
            vt[i] = p.vt[vindex[2][i]];
            //in_vt = (glm::vec2*)0x1;
        } else {
            vt[i] = in_vt.empty() ? generate_vt(v[i], glm::vec3()) : in_vt.at(i);
        }
        i++;
    }
    parse_vn(p, vindex, v, vn);
    if (in_vt.empty())
        correct_vt(vt);
    push_values(p, v, vn, vt);
}

void parse_vg(ObjContainer& p, const std::string& name = "")
{
    static int childNbr = 0;
    childNbr++;

    if (name == "") {
        p.currentGeometry = std::make_shared<Geometry>(p.container->GetComponent<Mesh>()->GetName() + "_Geometry_" + std::to_string(childNbr));
    } else {
        p.currentGeometry = std::make_shared<Geometry>(name);
    }
    p.container->GetComponent<Mesh>()->AddGeometry(p.currentGeometry, nullptr);
}

void correct_vt(glm::vec2* vt)
{
    glm::vec3 v[3]{};
    glm::vec3 texnormal { 0, 0, 0 };

    v[0] = glm::vec3(vt[0], 0.f);
    v[1] = glm::vec3(vt[1], 0.f);
    v[2] = glm::vec3(vt[2], 0.f);
    texnormal = glm::cross(v[1] - v[0], v[2] - v[0]);
    if (texnormal.z > 0) {
        if (vt[0].x < 0.25f)
            vt[0].x += 1.f;
        if (vt[1].x < 0.25f)
            vt[1].x += 1.f;
        if (vt[2].x < 0.25f)
            vt[2].x += 1.f;
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
    glm::vec3 v { 0 };
    for (uint8_t i = 0; i < 3 && (i + 1) < split.size(); ++i)
        v[i] = std::stof(split.at(i + 1));
    return v;
}

glm::vec2 parse_vec2(std::vector<std::string>& split)
{
    glm::vec2 v { 0 };
    for (uint8_t i = 0; i < 2 && (i + 1) < split.size(); ++i)
        v[i] = std::stof(split.at(i + 1));
    return v;
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

    split.erase(split.begin());
    faces = split.size() - 3 + 1;
    for (short i = 0; i < faces; ++i) {
        if (faces == 2 && i == 0) {
            parse_v(p,
                { split.at(0), split.at(i + 1), split.at(i + 2) },
                { glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1) });
        } else if (faces == 2 && i >= 1) {
            auto lesplit = std::vector<std::string>();
            parse_v(p,
                { split.at(0), split.at(i + 1), split.at(i + 2) },
                { glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0) });
        } else {
            parse_v(p,
                { split.at(0), split.at(i + 1), split.at(i + 2) }, {});
        }
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
        auto mtl { p.container->GetComponent<Mesh>()->GetGeometryMaterial(p.currentGeometry) };
        parse_vg(p, split[1]);
        p.container->GetComponent<Mesh>()->SetGeometryMaterial(p.currentGeometry, mtl);
    } else if (split[0] == "usemtl") {
        if (p.currentGeometry == nullptr || p.currentGeometry->Accessor(Geometry::AccessorKey::Position))
            parse_vg(p);
        p.container->GetComponent<Mesh>()->SetGeometryMaterial(
            p.currentGeometry,
            p.container->GetComponentByName<Material>(split.at(1)));
    } else if (split[0] == "mtllib") {
        auto mtllibAsset { std::make_shared<Assets::Asset>((p.path.parent_path() / split[1]).string()) };
        Assets::Parser::Parse(mtllibAsset);
        p.container += mtllibAsset;
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
    p.container->AddComponent(std::make_shared<Mesh>(path));
    auto l = 1;
    while (fgets(line, 4096, fd) != nullptr) {
        parse_line(p, line);
        l++;
    }
    fclose(fd);
}

void ParseOBJ(std::shared_ptr<Assets::Asset> container)
{
    ObjContainer p;

    p.path = container->GetUri().DecodePath();
    start_obj_parsing(p, p.path.string());
    container += p.container;
    auto scene(std::make_shared<Scene>(p.path.string()));
    auto node(std::make_shared<Node>(p.path.string() + "_node"));
    for (const auto& mesh : container->GetComponents<Mesh>()) {
        scene->GetRootNode()->AddSurface(mesh);
    }
    container->scenes.push_back(scene);
    container->SetLoaded(true);
}
