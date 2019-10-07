/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-07 10:03:28
*/

#include "parser/OBJ.hpp"
#include "AABB.hpp" // for AABB
#include "BoundingElement.hpp" // for BoundingElement
#include "Engine.hpp" // for M_PI
#include "Material.hpp" // for Material
#include "Mesh.hpp" // for Mesh
#include "MeshParser.hpp" // for MeshParser
#include "Vgroup.hpp" // for Vgroup, CVEC4
#include "glm/glm.hpp" // for s_vec3, s_vec2, glm::vec2, glm::vec3
#include "parser/InternalTools.hpp" // for t_obj_parser, count_char, split_...
#include "parser/MTLLIB.hpp" // for parse
#include <algorithm> // for max, min
#include <bits/exception.h> // for exception
#include <errno.h> // for errno
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <math.h> // for atan2
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fgets, fopen
#include <string.h> // for memset, strerror
#include <vector> // for vector

#ifdef _WIN32
#include <io.h>
#else
#include <sys/io.h>
#endif // for access, R_OK

//Add this parser to MeshParser !
auto __objParser = MeshParser::Add("obj", OBJ::parse);

static void push_values(t_obj_parser *p, glm::vec3 *v, glm::vec3 *vn, glm::vec2 *vt)
{
    unsigned i;
    CVEC4 ub{0, 0, 0, 0};

    i = 0;
    while (i < 3)
    {
        p->vg->boundingElement->min.x = std::min(v[i].x, p->vg->boundingElement->min.x);
        p->vg->boundingElement->min.y = std::min(v[i].y, p->vg->boundingElement->min.y);
        p->vg->boundingElement->min.z = std::min(v[i].z, p->vg->boundingElement->min.z);
        p->vg->boundingElement->max.x = std::max(v[i].x, p->vg->boundingElement->max.x);
        p->vg->boundingElement->max.y = std::max(v[i].y, p->vg->boundingElement->max.y);
        p->vg->boundingElement->max.z = std::max(v[i].z, p->vg->boundingElement->max.z);
        p->vg->boundingElement->center = (p->vg->boundingElement->min + p->vg->boundingElement->max) * 0.5f;
        p->vg->v.push_back(v[i]);
        p->vg->vt.push_back(vt[i]);
        ub.x = (vn[i].x + 1) * 0.5 * 255;
        ub.y = (vn[i].y + 1) * 0.5 * 255;
        ub.z = (vn[i].z + 1) * 0.5 * 255;
        ub.w = 255;
        p->vg->vn.push_back(ub);
        i++;
    }
}

static int get_vi(const std::vector<glm::vec2> &v, const std::string &str)
{
    int vindex;

    vindex = std::stoi(str);
    if (vindex < 0)
    {
        vindex = v.size() + vindex;
    }
    else
    {
        vindex -= 1;
    }
    if (vindex < 0 || static_cast<unsigned>(vindex) >= v.size())
    {
        return (-1);
    }
    return (vindex);
}

static int get_vi(const std::vector<glm::vec3> &v, const std::string &str)
{
    int vindex;

    vindex = std::stoi(str);
    if (vindex < 0)
    {
        vindex = v.size() + vindex;
    }
    else
    {
        vindex -= 1;
    }
    if (vindex < 0 || static_cast<unsigned>(vindex) >= v.size())
    {
        return (-1);
    }
    return (vindex);
}

static void parse_indice(t_obj_parser *p, std::vector<std::string> &split, int vindex[3][3])
{
    for (auto i = 0; i < 3; i++)
    {
        vindex[i][0] = -1;
        vindex[i][1] = -1;
        vindex[i][2] = -1;
    }
    for (auto i = 0u; i < split.size() && i < 3; i++)
    {
        //while (i < split.size() && i < 3) {
        auto fsplit = strsplit(split[i], '/');
        auto splitLen = fsplit.size();
        auto slashCount = count_char(split[i], '/');
        vindex[0][i] = -1;
        vindex[1][i] = -1;
        vindex[2][i] = -1;
        vindex[0][i] = get_vi(p->v, fsplit[0]);
        if (vindex[0][i] == -1)
        {
            return;
        }
        if ((splitLen == 3 && slashCount == 2) || (splitLen == 2 && slashCount == 1))
        {
            vindex[2][i] = get_vi(p->vt, fsplit[1]);
        }
        if (splitLen == 3 && slashCount == 2)
        {
            vindex[1][i] = get_vi(p->vn, fsplit[2]);
        }
        else if (splitLen == 2 && slashCount == 2)
        {
            vindex[1][i] = get_vi(p->vn, fsplit[1]);
        }
        //i++;
    }
}

static void parse_vn(t_obj_parser *p, int vindex[3][3], glm::vec3 v[3], glm::vec3 vn[3])
{
    short i;

    i = 0;
    while (i < 3)
    {
        if (vindex[1][i] != -1)
        {
            vn[i] = p->vn[vindex[1][i]];
        }
        else
        {
            vn[i] = generate_vn(v);
        }
        i++;
    }
}

void parse_v(t_obj_parser *p, std::vector<std::string> &split, glm::vec2 *in_vt)
{
    int vindex[3][3];
    glm::vec3 v[3];
    glm::vec3 vn[3];
    glm::vec2 vt[3];
    short i;

    parse_indice(p, split, vindex);
    i = 0;
    while (i < 3)
    {
        if (vindex[0][i] == -1)
        {
            return;
        }
        v[i] = p->v[vindex[0][i]];
        if (vindex[2][i] != -1)
        {
            vt[i] = p->vt[vindex[2][i]];
            in_vt = (glm::vec2 *)0x1;
        }
        else
        {
            vt[i] = in_vt != nullptr ? in_vt[i] : generate_vt(v[i], p->bbox.center);
        }
        i++;
    }
    parse_vn(p, vindex, v, vn);
    if (in_vt == nullptr)
    {
        correct_vt(vt);
    }
    push_values(p, v, vn, vt);
}

static void vt_min_max(std::shared_ptr<Vgroup> vg)
{
    vg->uvmin = glm::vec2(100000, 100000);
    vg->uvmax = glm::vec2(-100000, -100000);
    for (auto &vt : vg->vt)
    {
        if (vt.x < vg->uvmin.x)
        {
            vg->uvmin.x = vt.x;
        }
        if (vt.y < vg->uvmin.y)
        {
            vg->uvmin.y = vt.y;
        }
        if (vt.x > vg->uvmax.x)
        {
            vg->uvmax.x = vt.x;
        }
        if (vt.y > vg->uvmax.y)
        {
            vg->uvmax.y = vt.y;
        }
    }
    if (vg->uvmin.x == vg->uvmax.x)
    {
        vg->uvmin.x = 0;
        vg->uvmax.x = 1;
    }
    if (vg->uvmin.y == vg->uvmax.y)
    {
        vg->uvmin.y = 0;
        vg->uvmax.y = 1;
    }
}

void parse_vg(t_obj_parser *p, const std::string &name)
{
    if (!p->vg->v.empty())
    {
        static int childNbr = 0;
        childNbr++;
        vt_min_max(p->vg);
        //p->parent->add_child(p->vg);
        p->parent->AddVgroup(p->vg);
        if (name == "")
        {
            p->vg = Vgroup::Create(p->parent->Name() + "_vgroup_" + std::to_string(childNbr));
        }
        else
        {
            p->vg = Vgroup::Create(name);
        }
        //p->vg->set_material(Material::GetByName("default"));
    }
}

void correct_vt(glm::vec2 *vt)
{
    glm::vec3 v[3];
    glm::vec3 texnormal{0, 0, 0};

    v[0] = glm::vec3(vt[0], 0.f);
    v[1] = glm::vec3(vt[1], 0.f);
    v[2] = glm::vec3(vt[2], 0.f);
    texnormal = glm::cross(v[1] - v[0], v[2] - v[0]);
    if (texnormal.z > 0)
    {
        if (vt[0].x < 0.25f)
        {
            vt[0].x += 1.f;
        }
        if (vt[1].x < 0.25f)
        {
            vt[1].x += 1.f;
        }
        if (vt[2].x < 0.25f)
        {
            vt[2].x += 1.f;
        }
    }
}

glm::vec2 generate_vt(glm::vec3 v, glm::vec3 center)
{
    glm::vec2 vt{0, 0};
    glm::vec3 vec{0, 0, 0};

    vec = glm::normalize(center - v);
    vt.x = 0.5f + (atan2(vec.z, vec.x) / (2 * M_PI));
    vt.y = 0.5f + -vec.y * 0.5f;
    return (vt);
}

glm::vec3 generate_vn(glm::vec3 *v)
{
    return (glm::normalize(glm::cross(v[1] - v[0], v[2] - v[0])));
}

glm::vec3 parse_vec3(std::vector<std::string> &split)
{
    float v[3];
    unsigned i;

    i = 0;
    memset(v, 0, sizeof(float) * 3);
    while (i < 3)
    {
        if ((i + 1) >= split.size())
        {
            break;
        }
        v[i] = std::stof(split[i + 1]);
        i++;
    }
    return (glm::vec3(v[0], v[1], v[2]));
}

glm::vec2 parse_vec2(std::vector<std::string> &split)
{
    float v[2];
    unsigned i;

    i = 0;
    memset(v, 0, sizeof(float) * 2);
    while (i < 2)
    {
        if ((i + 1) >= split.size())
        {
            break;
        }
        v[i] = std::stof(split[i + 1]);
        i++;
    }
    return (glm::vec2(v[0], v[1]));
}

void parse_vtn(t_obj_parser *p, std::vector<std::string> &split)
{
    glm::vec3 v{0, 0, 0};
    glm::vec2 vt{0, 0};

    if (split[0] == "v")
    {
        v = parse_vec3(split);
        p->bbox.min.x = std::min(v.x, p->bbox.min.x);
        p->bbox.min.y = std::min(v.y, p->bbox.min.y);
        p->bbox.min.z = std::min(v.z, p->bbox.min.z);
        p->bbox.max.x = std::max(v.x, p->bbox.max.x);
        p->bbox.max.y = std::max(v.y, p->bbox.max.y);
        p->bbox.max.z = std::max(v.z, p->bbox.max.z);
        p->bbox.center = (p->bbox.min + p->bbox.max) * 0.5f;
        p->v.push_back(v);
    }
    else if (split[0] == "vn")
    {
        v = parse_vec3(split);
        p->vn.push_back(v);
    }
    else if (split[0] == "vt")
    {
        vt = parse_vec2(split);
        vt.y = vt.y;
        p->vt.push_back(vt);
    }
}

static void parse_f(t_obj_parser *p, std::vector<std::string> &split)
{
    short faces;
    short i;

    split.erase(split.begin());
    faces = split.size() - 3 + 1;
    i = 0;
    while (i < faces)
    {
        if (faces == 2 && i == 0)
        {
            auto lesplit = std::vector<std::string>({split[0], split[i + 1], split[i + 2]});
            auto levector = std::vector<glm::vec2>({glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1)});
            parse_v(p, lesplit, &levector[0]);
        }
        else if (faces == 2 && i >= 1)
        {
            auto lesplit = std::vector<std::string>({split[0], split[i + 1], split[i + 2]});
            auto levector = std::vector<glm::vec2>({glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0)});
            parse_v(p, lesplit, &levector[0]);
        }
        else
        {
            auto lesplit = std::vector<std::string>({split[0], split[i + 1], split[i + 2]});
            parse_v(p, lesplit, nullptr);
        }
        i++;
    }
}

#include <Debug.hpp>

static void parse_line(t_obj_parser *p, const char *line)
{
    auto split = strsplitwspace(line);
    if (split.empty() || split[0][0] == '#')
    {
        return;
    }
    if (split[0][0] == 'v')
    {
        parse_vtn(p, split);
    }
    else if (split[0][0] == 'f')
    {
        parse_f(p, split);
    }
    else if (split[0][0] == 'g' || split[0][0] == 'o')
    {
        auto mtlIndex(p->vg->MaterialIndex());
        parse_vg(p, split[1]);
        p->vg->SetMaterialIndex(mtlIndex);
    }
    else if (split[0] == "usemtl")
    {
        if (!p->vg->v.empty())
            parse_vg(p);
        auto mtl(Material::GetByName(split[1]));
        p->parent->AddMaterial(mtl);
        auto mtlIndex(p->parent->GetMaterialIndex(mtl));
        p->vg->SetMaterialIndex(mtlIndex);
    }
    else if (split[0] == "mtllib")
    {
        MTLLIB::parse(p->path_split[0] + split[1]);
    }
}

static void start_obj_parsing(t_obj_parser *p, const std::string &name, const std::string &path)
{
    char line[4096];

    if (access(path.c_str(), R_OK) != 0)
    {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((p->fd = fopen(path.c_str(), "r")) == nullptr)
    {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    p->parent = Mesh::Create(name);
    p->vg = Vgroup::Create(name + "_child 0");
    p->vg->boundingElement = new AABB(p->bbox);
    auto l = 1;
    while (fgets(line, 4096, p->fd) != nullptr)
    {
        try
        {
            parse_line(p, line);
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("Error at line : " + std::to_string(l) + " " + e.what());
        }
        l++;
    }
    fclose(p->fd);
    if (!p->vg->v.empty() != 0u)
    {
        parse_vg(p);
    }
    else
    {
        throw std::runtime_error(std::string("Invalid OBJ"));
    }
}

std::shared_ptr<Mesh> OBJ::parse(const std::string &name, const std::string &path)
{
    t_obj_parser p;

    p.path_split = split_path(path);
    try
    {
        start_obj_parsing(&p, name, path);
    }
    catch (std::exception &e)
    {
        throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
        return (nullptr);
    }
    p.parent->bounding_element = new AABB(p.bbox);
    return (p.parent);
}
