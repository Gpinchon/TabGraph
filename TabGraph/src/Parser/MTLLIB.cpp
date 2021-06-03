/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:15
*/

#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"
#include "Assets/Image.hpp" // for ImageParser
#include "Debug.hpp"
#include "Material/Material.hpp" // for Material
#include "Material/MetallicRoughness.hpp"
#include "Material/SpecularGlossiness.hpp"
#include "Parser/InternalTools.hpp" // for parse_vec3, t_obj_parser, strspl...
#include <filesystem>
#include <glm/glm.hpp> // for s_vec3, glm::vec3, vec3_fdiv, CLAMP
#include <memory> // for shared_ptr, allocator, __shared_...
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fgets, fclose, fopen
#include <stdlib.h> // for errno
#include <string.h> // for strerror
#include <vector> // for vector

void ParseMTLLIB(const std::shared_ptr<Asset>);

auto MTLLIBMimeExtension{
    AssetsParser::AddMimeExtension("model/mtllib", ".mtl") //not standard but screw it.
};

auto MTLLIBMimesParsers{
    AssetsParser::Add("model/mtllib", ParseMTLLIB)
};

#ifdef _WIN32
#include <io.h>
#ifndef R_OK
#define R_OK 4
#endif
#else
#include <sys/io.h>
#endif // for access, R_OK

std::shared_ptr<MetallicRoughness> GetOrCreateMetallicRoughnessExtention(std::shared_ptr<Material> mtl)
{
    auto extension = std::static_pointer_cast<MetallicRoughness>(mtl->GetExtension("MetallicRoughness"));
    if (extension == nullptr) {
        extension = Component::Create<MetallicRoughness>();
        extension->SetMetallic(0.f);
        extension->SetRoughness(0.f);
        mtl->AddExtension(extension);
    }
    return extension;
}

std::shared_ptr<SpecularGlossiness> GetOrCreateSpecularGlossinessExtention(std::shared_ptr<Material> mtl)
{
    auto extension = std::static_pointer_cast<SpecularGlossiness>(mtl->GetExtension("SpecularGlossiness"));
    if (extension == nullptr) {
        extension = Component::Create<SpecularGlossiness>();
        extension->SetSpecular(glm::vec3(0));
        mtl->AddExtension(extension);
    }
    return extension;
}

static inline void parse_color(std::vector<std::string>& split, std::shared_ptr<Material> mtl)
{
    if (split[0] == "Kd") {
        mtl->SetDiffuse(parse_vec3(split));
    } else if (split[0] == "Ks") {
        //mtl->SetSpecular(parse_vec3(split) / (1 + (1 - mtl->Metallic()) * 24));
        auto extension = GetOrCreateSpecularGlossinessExtention(mtl);
        extension->SetSpecular(parse_vec3(split) / 100.f);
    } else if (split[0] == "Ke") {
        mtl->SetEmissive(parse_vec3(split));
    }
}

#include "Texture/Texture2D.hpp"

std::unordered_map<std::string, std::shared_ptr<Texture2D>> textureDatabase;

auto GetOrCreateImage(std::filesystem::path path, std::shared_ptr<Asset> container)
{
    auto absolutePath = std::filesystem::absolute(path).string();
    if (textureDatabase[absolutePath] != nullptr)
        return textureDatabase[absolutePath];
    auto asset = Component::Create<Asset>(absolutePath);
    asset->parsingOptions = container->parsingOptions;
    return textureDatabase[absolutePath] = Component::Create<Texture2D>(asset);
}

static inline void parse_texture(std::shared_ptr<Material> mtl, const std::string& textureType, std::filesystem::path path, std::shared_ptr<Asset> container)
{
    try {
        if (textureType == "map_Kd") {
            mtl->SetTextureDiffuse(GetOrCreateImage(path, container));
            /*if (mtl->TextureDiffuse()->values_per_pixel() == 4) {
                mtl->SetOpacityMode(Material::OpacityMode::Blend);
            }*/
        } else if (textureType == "map_Ks") {
            auto extension = GetOrCreateSpecularGlossinessExtention(mtl);
            extension->SetTextureSpecular(GetOrCreateImage(path, container));
        } else if (textureType == "map_Ke") {
            mtl->SetEmissive(glm::vec3(1));
            mtl->SetTextureEmissive(GetOrCreateImage(path, container));
        } else if (textureType == "map_Nh") {
            mtl->SetTextureHeight(GetOrCreateImage(path, container));
        } else if (textureType == "map_No") {
            mtl->SetTextureAO(GetOrCreateImage(path, container));
        } else if (textureType == "map_Nr") {
            auto extension = GetOrCreateMetallicRoughnessExtention(mtl);
            extension->SetTextureRoughness(GetOrCreateImage(path, container));
            if (extension->GetRoughness() == 0.f)
                extension->SetRoughness(1);
        } else if (textureType == "map_Nm") {
            auto extension = GetOrCreateMetallicRoughnessExtention(mtl);
            extension->SetTextureMetallic(GetOrCreateImage(path, container));
            if (extension->GetMetallic() == 0.f)
                extension->SetMetallic(1);
        } else if (textureType == "map_bump" || textureType == "map_Bump") {
            mtl->SetTextureNormal(GetOrCreateImage(path, container));
        }
    } catch (std::exception& e) {
        debugLog("Error while parsing texture : " << e.what());
    }
}

static inline void parse_number(std::vector<std::string>& split, std::shared_ptr<Material> mtl)
{

    if (split[0] == "Np") {
        mtl->SetParallax(std::stof(split[1]));
    } else if (split[0] == "Ns") {
        auto extension = GetOrCreateSpecularGlossinessExtention(mtl);
        extension->SetGlossiness(glm::clamp(std::stof(split[1]), 0.f, 250.f) / 250.f);
        //mtl->SetRoughness(glm::clamp(1.f / (1.f + std::stof(split[1])) * 50.f, 0.f, 1.f));
    } else if (split[0] == "Nr") {
        auto extension = GetOrCreateMetallicRoughnessExtention(mtl);
        extension->SetRoughness(std::stof(split[1]));
    } else if (split[0] == "Nm") {
        auto extension = GetOrCreateMetallicRoughnessExtention(mtl);
        extension->SetMetallic(std::stof(split[1]));
    } else if (split[0] == "Ni") {
        float ior;
        ior = std::stof(split[1]);
        mtl->SetIor(ior);
        //ior = (ior - 1) / (ior + 1);
        //ior *= ior;
        //mtl->SetSpecular(glm::vec3(ior, ior, ior));
    } else if (split[0] == "Tr") {
        mtl->SetOpacity(1 - std::stof(split[1]));
        if (mtl->GetOpacity() < 1.f) {
            mtl->SetOpacityMode(Material::OpacityMode::Blend);
            mtl->SetDoubleSided(true);
        }
    }
}

static inline std::shared_ptr<Material> parse_mtl(FILE* fd, std::string& name, const std::filesystem::path& basePath, std::shared_ptr<Asset> container)
{
    char line[4096];
    auto mtl = Component::Create<Material>(name);
    fpos_t pos = 0;
    fgetpos(fd, &pos);
    while (fgets(line, 4096, fd) != nullptr) {
        try {
            auto msplit = strsplitwspace(line);
            if (msplit.size() > 1 && msplit[0][0] != '#') {
                if (msplit[0][0] == 'K') {
                    parse_color(msplit, mtl);
                } else if (msplit[0][0] == 'N' || msplit[0][0] == 'T') {
                    parse_number(msplit, mtl);
                } else if (msplit[0].find("map_") != std::string::npos) {
                    parse_texture(mtl, msplit.at(0), basePath / msplit.at(1), container);
                } else if (msplit[0] == "newmtl") {
                    fsetpos(fd, &pos);
                    break;
                }
                fgetpos(fd, &pos);
            }
        } catch (std::exception& e) {
            throw std::runtime_error("Error while parsing " + name + " at line \"" + line + "\" : " + e.what());
        }
    }
    return mtl;
}

void ParseMTLLIB(std::shared_ptr<Asset> container)
{
    char line[4096];
    auto uri{ container->GetUri() };
    //std::shared_ptr<AssetsContainer> container = Component::Create<AssetsContainer>();

    if (access(uri.GetPath().string().c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + uri.GetPath().string() + " : " + strerror(errno));
    }
    FILE* fd = nullptr;
    if ((fd = fopen(uri.GetPath().string().c_str(), "rb")) == nullptr) {
        throw std::runtime_error(std::string("Can't open ") + uri.GetPath().string() + " : " + strerror(errno));
    }
    auto basePath = std::filesystem::absolute(uri.GetPath()).parent_path();
    auto l = 1;
    while (fgets(line, 4096, fd) != nullptr) {
        try {
            auto split = strsplitwspace(line);
            if (split.size() > 1 && split[0][0] != '#') {
                if (split[0] == "newmtl") {
                    container->AddComponent(parse_mtl(fd, split[1], basePath, container));
                }
            }
        } catch (std::exception& e) {
            throw std::runtime_error("Error while parsing " + uri.GetPath().string() + " at line " + std::to_string(l) + " : " + e.what());
        }
        l++;
    }
    fclose(fd);
    textureDatabase.clear();
}
