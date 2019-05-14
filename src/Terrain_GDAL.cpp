/*
* @Author: gpi
* @Date:   2019-03-26 12:03:23
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-14 09:37:44
*/

#include "Terrain.hpp"
#include "Vgroup.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Debug.hpp"
#include "TextureParser.hpp"
#include "parser/InternalTools.hpp"
#include <gdal_priv.h>

Terrain::Terrain(const std::string &name) : Mesh(name)
{

}

#include <fstream>

#include <limits>

std::shared_ptr<Terrain> Terrain::create(const std::string& name,
    VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> texture)
{
    auto terrain = std::shared_ptr<Terrain>(new Terrain(name));
    terrain->_terrainData = texture;
    terrain->_terrainResolution = resolution;
    Mesh::add(terrain);
    Renderable::add(terrain);
    Node::add(terrain);
    auto vg = Vgroup::create(name + "vgroup");
    vg->v.resize(uint32_t(resolution.x * resolution.y));
    vg->vn.resize(vg->v.size());
    vg->vt.resize(vg->v.size());
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    std::ofstream myfile;
    myfile.open(name + ".txt");
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
                myfile << z << " ";
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
        myfile << std::endl;
    }
    myfile.close();
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

std::shared_ptr<Terrain>  Terrain::create(const std::string& name, VEC2 resolution, const std::string &path)
{
    GDALAllRegister();
    auto data = (GDALDataset*)GDALOpen(path.c_str(), GA_ReadOnly );
    if (data == nullptr) {
        throw std::runtime_error(path + " : Could not open");
    }
    auto    band = data->GetRasterBand(1);
    if (band == nullptr) {
        throw std::runtime_error(path + " : Could not get raster band.");
    }
    double  gt[6];
    auto    err1 = data->GetGeoTransform(gt);
    VEC2    size {float(band->GetXSize()), float(band->GetYSize())};
    float   *scanLine = (float *) CPLMalloc(sizeof(float) * size.x * size.y);
    auto    err = band->RasterIO(GF_Read, 0, 0, size.x, size.y,
                      scanLine, size.x, size.y, GDT_Float32,
                      0, 0);
    if (err1 != CE_None || err != CE_None)
    {
        debugLog("Something went wrong with : " + path);
        return Terrain::create(name, resolution, new_vec3(1, 1, 1),  TextureParser::parse(name + "_terrainData", path));
    }
    std::string proj = data->GetProjectionRef();
    debugLog(proj);
    GDALClose(data);
    auto t = Texture::create(name + "_terrainData", size, GL_TEXTURE_2D,
    GL_RED, GL_COMPRESSED_RED, GL_FLOAT, scanLine);
    CPLFree(scanLine);
    return Terrain::create(name, resolution, new_vec3(size.x * gt[1], 1, size.y * -gt[5]), t);
}
