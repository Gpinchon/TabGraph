/*
* @Author: gpi
* @Date:   2019-03-26 12:03:23
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-05-08 22:18:27
*/

#include "Terrain.hpp"
#include "Vgroup.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Debug.hpp"
#include "parser/TerrainData.hpp"
#include "parser/InternalTools.hpp"

Terrain::Terrain(const std::string &name) : Mesh(name)
{

}

#include <limits>

std::shared_ptr<Terrain> Terrain::create(const std::string& name,
    VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> texture)
{
    auto terrain = std::shared_ptr<Terrain>(new Terrain(name));
    terrain->_terrainMap = texture;
    Mesh::add(terrain);
    Renderable::add(terrain);
    Node::add(terrain);
    auto vg = Vgroup::create(name + "vgroup");
    vg->v.resize(uint32_t(resolution.x * resolution.y));
    vg->vn.resize(vg->v.size());
    vg->vt.resize(vg->v.size());
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (auto y = 0.f; y < resolution.y; y++) {
        for (auto x = 0.f; x < resolution.x; x++) {
            auto uv = new_vec2(x / resolution.x, y / resolution.y);
            auto z = 0.f;
            if (texture) {
                //z = texture->sample(uv).x;
               
                VEC2    texUV;
                texUV.x = uv.x * texture->size().x;
                texUV.y = uv.y * texture->size().y;
                z = ((float*)texture->texelfetch(texUV))[0];
                
                minZ = z < minZ ? z : minZ;
                maxZ = z > maxZ ? z : maxZ;
                std::cout << z << " ";
            }
            vg->vt.at(uint32_t(x + y * resolution.x)) = uv;
            auto &v3 = vg->v.at(uint32_t(x + y * resolution.x));
            v3 = new_vec3(uv.x * scale.x - scale.x / 2.f, z * scale.y, uv.y * scale.z - scale.z / 2.f);

            if (x < resolution.x - 1 && y < resolution.y - 1) {
                vg->i.push_back(uint32_t(x + y * resolution.x));
                vg->i.push_back(uint32_t(x + (y + 1) * resolution.x));
                vg->i.push_back(uint32_t((x + 1) + (y + 1) * resolution.x));

                vg->i.push_back(uint32_t(x + y * resolution.x));
                vg->i.push_back(uint32_t((x + 1) + (y + 1) * resolution.x));
                vg->i.push_back(uint32_t((x + 1) + y * resolution.x));
            }
        }
        std::cout << std::endl;
    }
    auto medZ = (maxZ + minZ) / 2.f * scale.y;
    for (auto &v : vg->v)
        v.y -= medZ;
    for (auto i = 0u; i * 3 < vg->i.size(); i++)
    {
        auto i0 = vg->i.at(i * 3 + 0);
        auto i1 = vg->i.at(i * 3 + 1);
        auto i2 = vg->i.at(i * 3 + 2);
        auto v0 = vg->v.at(i0);
        auto v1 = vg->v.at(i1);
        auto v2 = vg->v.at(i2);
        auto &n0 = vg->vn.at(i0);
        auto &n1 = vg->vn.at(i1);
        auto &n2 = vg->vn.at(i2);

        VEC3 N0 = new_vec3((n0.x / 255.f) * 2 - 1, (n0.y / 255.f) * 2 - 1, (n0.z / 255.f) * 2 - 1);
        VEC3 N1 = new_vec3((n1.x / 255.f) * 2 - 1, (n1.y / 255.f) * 2 - 1, (n1.z / 255.f) * 2 - 1);
        VEC3 N2 = new_vec3((n2.x / 255.f) * 2 - 1, (n2.y / 255.f) * 2 - 1, (n2.z / 255.f) * 2 - 1);
        VEC3 N;
        N = vec3_cross(vec3_sub(v1, v0), vec3_sub(v2, v0));
        N = vec3_normalize(N);
        if ((N0.x + N0.y + N0.z) == 0) {
            N0 = N;
        }
        else {
            N0 = vec3_add(N0, N);
            N0 = vec3_fdiv(N0, 2);
            N0 = vec3_normalize(N0);
        }
        if ((N1.x + N1.y + N1.z) == 0) {
            N1 = N;
        }
        else {
            N1 = vec3_add(N1, N);
            N1 = vec3_fdiv(N1, 2);
            N1 = vec3_normalize(N1);
        }
        if ((N2.x + N2.y + N2.z) == 0) {
            N2 = N;
        }
        else {
            N2 = vec3_add(N2, N);
            N2 = vec3_fdiv(N2, 2);
            N2 = vec3_normalize(N2);
        }
        n0.x = ((N0.x + 1) * 0.5) * 255.f;
        n0.y = ((N0.y + 1) * 0.5) * 255.f;
        n0.z = ((N0.z + 1) * 0.5) * 255.f;
        n1.x = ((N1.x + 1) * 0.5) * 255.f;
        n1.y = ((N1.y + 1) * 0.5) * 255.f;
        n1.z = ((N1.z + 1) * 0.5) * 255.f;
        n2.x = ((N2.x + 1) * 0.5) * 255.f;
        n2.y = ((N2.y + 1) * 0.5) * 255.f;
        n2.z = ((N2.z + 1) * 0.5) * 255.f;
    }
    auto mtl = Material::create("default_terrain");
    //mtl->set_texture_albedo(texture);
    //mtl->set_texture_roughness(texture);
    mtl->albedo = new_vec3(0.5, 0.5, 0.5);
    mtl->roughness = 0.5;
    vg->set_material(mtl);
    terrain->add(vg);
    return terrain;
}

void    printBTHeader(BTHeader &header)
{
    std::cout <<
    "BTHeader :\n" <<
    " Marker :        " << header.marker << "\n" <<
    " columns :       " << header.columns << "\n" <<
    " rows :          " << header.rows << "\n" <<
    " dataSize :      " << header.dataSize << "\n" <<
    " fPoint :        " << header.fPoint << "\n" <<
    " hUnits :        " << header.hUnits << "\n" <<
    " UTMZone :       " << header.UTMZone << "\n" <<
    " datum :         " << header.datum << "\n" <<
    " leftExtent :    " << header.leftExtent << "\n" <<
    " rightExtent :   " << header.rightExtent << "\n" <<
    " bottomExtent :  " << header.bottomExtent << "\n" <<
    " topExtent :     " << header.topExtent << "\n" <<
    " projection :    " << header.projection << "\n" <<
    " verticalScale : " << header.verticalScale << "\n" <<
    " padding :       " << header.padding << "\n" << std::endl;
}

std::shared_ptr<Terrain> Terrain::create(const std::string& name, VEC2 resolution, const std::string &path)
{
    auto    terrainScale = new_vec3(1, 0.00001, 1);
    if (fileFormat(path) == "bt" || fileFormat(path) == "BT") {
        BTHeader    header;
        size_t      readSize;
        auto fd = openFile(path);

        if ((readSize = fread(&header, 1, sizeof(BTHeader), fd) != sizeof(BTHeader))) {
            fclose(fd);
            throw std::runtime_error(std::string("[ERROR] ") + path + " : " +
                "Invalid file header, expected size " + std::to_string(sizeof(BTHeader)) + " got " + std::to_string(readSize));
        }
        fclose(fd);
        printBTHeader(header);        
        //terrainScale.x = measureHaversine(header.leftExtent, )
        terrainScale.x = header.topExtent - header.bottomExtent;
        terrainScale.y = header.verticalScale == 0 ? 1.0 : header.verticalScale;
        terrainScale.z = header.rightExtent - header.leftExtent;
        if (header.hUnits == 2) {
            terrainScale.x *= 0.3048;
            terrainScale.y *= 0.3048;
            terrainScale.z *= 0.3048;
        }
        else if (header.hUnits == 3) {
            terrainScale.x *= 1200.0 / 3937.0;
            terrainScale.y *= 1200.0 / 3937.0;
            terrainScale.z *= 1200.0 / 3937.0;
        }
    }
    return (Terrain::create(name, resolution, terrainScale, TerrainData::parse(name + "_heightMap", path)));
}

