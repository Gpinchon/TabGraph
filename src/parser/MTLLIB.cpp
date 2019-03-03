/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MTLLIB.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/03 12:03:11 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/MTLLIB.hpp"
#include "Engine.hpp"
#include "Material.hpp"
//#include "parser/BMP.hpp"
#include "parser/InternalTools.hpp"
#include <stdexcept>
#include <unistd.h>

void parse_color(std::vector<std::string>& split, std::shared_ptr<Material> mtl)
{
    if (split[0] == "Kd") {
        mtl->albedo = parse_vec3(split);
    } else if (split[0] == "Ks") {
        mtl->specular = vec3_fdiv(parse_vec3(split),
            1 + (1 - mtl->metallic) * 24);
    } else if (split[0] == "Ke") {
        mtl->emitting = parse_vec3(split);
    }
}

void parse_texture(t_obj_parser* p, std::vector<std::string>& split, std::shared_ptr<Material> mtl)
{
    std::string path(p->path_split[0]);

    path += split[1];
    if (split[0] == "map_Kd") {
        mtl->set_texture_albedo(Texture::parse(path, path));
    } else if (split[0] == "map_Ks") {
        mtl->set_texture_specular(Texture::parse(path, path));
    } else if (split[0] == "map_Ke") {
        mtl->set_texture_emitting(Texture::parse(path, path));
    } else if (split[0] == "map_Nh") {
        mtl->set_texture_height(Texture::parse(path, path));
    } else if (split[0] == "map_No") {
        mtl->set_texture_ao(Texture::parse(path, path));
    } else if (split[0] == "map_Nr") {
        mtl->set_texture_roughness(Texture::parse(path, path));
    } else if (split[0] == "map_Nm") {
        mtl->set_texture_metallic(Texture::parse(path, path));
    } else if (split[0] == "map_bump" || split[0] == "map_Bump") {
        mtl->set_texture_normal(Texture::parse(path, path));
    }
}

void parse_number(std::vector<std::string>& split, std::shared_ptr<Material> mtl)
{

    if (split[0] == "Np") {
        mtl->parallax = std::stof(split[1]);
    } else if (split[0] == "Ns") {
        mtl->roughness = CLAMP(1.f / (1.f + std::stof(split[1])) * 50.f, 0, 1);
    } else if (split[0] == "Nr") {
        mtl->roughness = std::stof(split[1]);
    } else if (split[0] == "Nm") {
        mtl->metallic = std::stof(split[1]);
    } else if (split[0] == "Ni") {
        float ior;
        ior = std::stof(split[1]);
        mtl->ior = ior;
        ior = (ior - 1) / (ior + 1);
        ior *= ior;
        mtl->specular = new_vec3(ior, ior, ior);
    } else if (split[0] == "Tr") {
        mtl->alpha = 1 - std::stof(split[1]);
    }
}

void parse_mtl(t_obj_parser* p, std::string& name)
{
    if (Material::get_by_name(name) != nullptr) {
        return;
    }
    char line[4096];
    auto mtl = Material::create(name);
    while (fgets(line, 4096, p->fd) != nullptr) {
        auto msplit = strsplitwspace(line);
        if (msplit.size() > 1 && msplit[0][0] != '#') {
            if (msplit[0][0] == 'K') {
                parse_color(msplit, mtl);
            } else if (msplit[0][0] == 'N' || msplit[0][0] == 'T') {
                parse_number(msplit, mtl);
            } else if (msplit[0].find("map_") != std::string::npos) {
                parse_texture(p, msplit, mtl);
            } else if (msplit[0] == "newmtl") {
                parse_mtl(p, msplit[1]);
            }
        }
    }
}

bool MTLLIB::parse(const std::string& path)
{
    char line[4096];
    t_obj_parser p;

    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    if ((p.fd = fopen(path.c_str(), "r")) == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
    }
    p.path_split = split_path(path);
    while (fgets(line, 4096, p.fd) != nullptr) {
        auto split = strsplitwspace(line);
        if (split.size() > 1 && split[0][0] != '#') {
            if (split[0] == "newmtl") {
                parse_mtl(&p, split[1]);
            }
        }
    }
    fclose(p.fd);
    return (true);
}
