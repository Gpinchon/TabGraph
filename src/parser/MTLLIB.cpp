/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-09-09 17:39:10
*/

#include "parser/MTLLIB.hpp"
#include "Material.hpp" // for Material
#include "TextureParser.hpp" // for TextureParser
#include "glm/glm.hpp" // for s_vec3, glm::vec3, vec3_fdiv, CLAMP
#include "parser/InternalTools.hpp" // for parse_vec3, t_obj_parser, strspl...
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <memory> // for shared_ptr, allocator, __shared_...
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fgets, fclose, fopen
#include <stdlib.h> // for errno
#include <string.h> // for strerror
#include <vector> // for vector

#ifdef _WIN32
#include <io.h>
#else
#include <sys/io.h>
#endif // for access, R_OK

void parse_color(std::vector<std::string> &split, std::shared_ptr<Material> mtl)
{
    if (split[0] == "Kd")
    {
        mtl->albedo = parse_vec3(split);
    }
    else if (split[0] == "Ks")
    {
        mtl->specular = parse_vec3(split) / (1 + (1 - mtl->metallic) * 24);
    }
    else if (split[0] == "Ke")
    {
        mtl->emitting = parse_vec3(split);
    }
}

void parse_texture(t_obj_parser *p, std::vector<std::string> &split, std::shared_ptr<Material> mtl)
{
    std::string path(p->path_split[0]);

    path += split[1];
    if (split[0] == "map_Kd")
    {
        mtl->set_texture_albedo(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_Ks")
    {
        mtl->set_texture_specular(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_Ke")
    {
        mtl->set_texture_emitting(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_Nh")
    {
        mtl->set_texture_height(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_No")
    {
        mtl->set_texture_ao(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_Nr")
    {
        mtl->set_texture_roughness(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_Nm")
    {
        mtl->set_texture_metallic(TextureParser::parse(path, path));
    }
    else if (split[0] == "map_bump" || split[0] == "map_Bump")
    {
        mtl->set_texture_normal(TextureParser::parse(path, path));
    }
}

void parse_number(std::vector<std::string> &split, std::shared_ptr<Material> mtl)
{

    if (split[0] == "Np")
    {
        mtl->parallax = std::stof(split[1]);
    }
    else if (split[0] == "Ns")
    {
        mtl->roughness = glm::clamp(1.f / (1.f + std::stof(split[1])) * 50.f, 0.f, 1.f);
    }
    else if (split[0] == "Nr")
    {
        mtl->roughness = std::stof(split[1]);
    }
    else if (split[0] == "Nm")
    {
        mtl->metallic = std::stof(split[1]);
    }
    else if (split[0] == "Ni")
    {
        float ior;
        ior = std::stof(split[1]);
        mtl->ior = ior;
        ior = (ior - 1) / (ior + 1);
        ior *= ior;
        mtl->specular = glm::vec3(ior, ior, ior);
    }
    else if (split[0] == "Tr")
    {
        mtl->alpha = 1 - std::stof(split[1]);
    }
}

#include <iostream>

auto parse_mtl(t_obj_parser *p, std::string &name)
{
    char line[4096];
    auto mtl = Material::Create(name);
    auto previousOffset(ftell(p->fd));
    while (fgets(line, 4096, p->fd) != nullptr)
    {
        try
        {
            auto msplit = strsplitwspace(line);
            if (msplit.size() > 1 && msplit[0][0] != '#')
            {
                if (msplit[0][0] == 'K')
                {
                    parse_color(msplit, mtl);
                }
                else if (msplit[0][0] == 'N' || msplit[0][0] == 'T')
                {
                    parse_number(msplit, mtl);
                }
                else if (msplit[0].find("map_") != std::string::npos)
                {
                    parse_texture(p, msplit, mtl);
                }
                else if (msplit[0] == "newmtl")
                {
                    fseek(p->fd, previousOffset, SEEK_SET);
                    break;
                    //parse_mtl(p, msplit[1]);
                }
            }
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("Error while parsing " + name + " at line \"" + line + "\" : " + e.what());
        }
        previousOffset = ftell(p->fd);
    }
    return mtl;
}

std::map<std::string, std::shared_ptr<Material>> MTLLIB::parse(const std::string &path)
{
    char line[4096];
    t_obj_parser p;
    std::map<std::string, std::shared_ptr<Material>> materials;

    if (access(path.c_str(), R_OK) != 0)
    {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((p.fd = fopen(path.c_str(), "r")) == nullptr)
    {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    p.path_split = split_path(path);
    auto l = 1;
    while (fgets(line, 4096, p.fd) != nullptr)
    {
        try
        {
            auto split = strsplitwspace(line);
            if (split.size() > 1 && split[0][0] != '#')
            {
                if (split[0] == "newmtl")
                {
                    auto mtl(parse_mtl(&p, split[1]));
                    materials[mtl->Name()] = mtl;
                }
            }
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("Error while parsing " + path + " at line " + std::to_string(l) + " : " + e.what());
        }
        l++;
    }
    fclose(p.fd);
    return materials;
}
