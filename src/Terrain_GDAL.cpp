/*
* @Author: gpi
* @Date:   2019-03-26 12:03:23
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:36:56
*/

#include <GL/glew.h>           // for GL_COMPRESSED_RED, GL_FLOAT, GL_RED
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <gdal_priv.h>         // for GDALRasterBand, GDALDataset
#include <math.h>              // for abs
#include <stdint.h>            // for uint32_t
#include <stdlib.h>            // for abs
#include <array>               // for array
#include <iostream>            // for operator<<, basic_ostream<>::__ostream...
#include <limits>              // for numeric_limits
#include <memory>              // for shared_ptr, __shared_ptr_access, alloc...
#include <stdexcept>           // for runtime_error
#include <string>              // for operator+, string
#include <vector>              // for vector
#include "Debug.hpp"           // for debugLog
#include "Material.hpp"        // for Material
#include "Mesh.hpp"            // for Mesh
#include "Node.hpp"            // for Node
#include "Quadtree.hpp"        // for Quadtree
#include "Renderable.hpp"      // for Renderable
#include "Terrain.hpp"         // for Terrain
#include "Texture.hpp"         // for Texture
#include "TextureParser.hpp"   // for TextureParser
#include "cpl_conv.h"          // for CPLMalloc, CPLFree
#include "cpl_error.h"         // for CE_None
#include "gdal.h"              // for GDALAllRegister, GDALClose, GDALOpen
#include "glm"               // for s_vec3, glm::vec2, s_vec2, glm::vec3, glm::vec3

Terrain::Terrain(const std::string &name) : Mesh(name)
{

}

void    Subdivide(Quadtree<std::array<glm::vec3, 4>> *tree, std::shared_ptr<Texture> texture, glm::vec3 scale)
{
    if (tree == nullptr)
        return;
    bool subdivided = false;
    for (auto index = 0u; index < tree->Data().size(); index++) {
        if (subdivided)
            index--;
        subdivided = false;
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float medY = 0;
        float delta = 0;
        for (auto v : tree->Data().at(index)) {
            minY = v.y < minY ? v.y : minY;
            maxY = v.y > maxY ? v.y : maxY;
            medY += v.y * 0.25;
        }
        if (abs(maxY - minY) < 0.01)
            continue;
        delta = (maxY - minY) / maxY;
        /*for (auto v : tree->Data().at(index))
            delta += abs(v.y - minY) / abs(maxY - minY) / 4.0;*/
        if (delta < 0.1) {
            continue;
        }
        //std::cout << delta << std::endl;
        auto uv4 = glm::vec2(tree->Mid().x, tree->Max().y);
        auto uv5 = glm::vec2(tree->Max().x, tree->Mid().y);
        auto uv6 = glm::vec2(tree->Mid().x, tree->Min().y);
        auto uv7 = glm::vec2(tree->Min().x, tree->Mid().y);
        auto uv8 = glm::vec2(tree->Mid().x, tree->Mid().y);
        auto v0 = tree->Data().at(index).at(0);
        auto v1 = tree->Data().at(index).at(1);
        auto v2 = tree->Data().at(index).at(2);
        auto v3 = tree->Data().at(index).at(3);
        auto v4 = glm::vec3(uv4.x, ((float*)texture->texelfetch(glm::vec2(uv4.x / scale.x * 0.5 + 1, uv4.y / scale.z * 0.5 + 1)))[0], uv4.y);
        auto v5 = glm::vec3(uv5.x, ((float*)texture->texelfetch(glm::vec2(uv5.x / scale.x * 0.5 + 1, uv5.y / scale.z * 0.5 + 1)))[0], uv5.y);
        auto v6 = glm::vec3(uv6.x, ((float*)texture->texelfetch(glm::vec2(uv6.x / scale.x * 0.5 + 1, uv6.y / scale.z * 0.5 + 1)))[0], uv6.y);
        auto v7 = glm::vec3(uv7.x, ((float*)texture->texelfetch(glm::vec2(uv7.x / scale.x * 0.5 + 1, uv7.y / scale.z * 0.5 + 1)))[0], uv7.y);
        auto v8 = glm::vec3(uv8.x, ((float*)texture->texelfetch(glm::vec2(uv8.x / scale.x * 0.5 + 1, uv8.y / scale.z * 0.5 + 1)))[0], uv8.y);
        tree->Data().erase(tree->Data().begin() + index);
        /*std::cout << "v4 " << v4.x << " " << v4.y << " " << v4.z*/std::cout << uv4.x / scale.x * 0.5 + 1 << " " << uv4.y / scale.z * 0.5 + 1 << std::endl;
        /*std::cout << "v5 " << v5.x << " " << v5.y << " " << v5.z*/std::cout << uv5.x / scale.x * 0.5 + 1 << " " << uv5.y / scale.z * 0.5 + 1 << std::endl;
        /*std::cout << "v6 " << v6.x << " " << v6.y << " " << v6.z*/std::cout << uv6.x / scale.x * 0.5 + 1 << " " << uv6.y / scale.z * 0.5 + 1 << std::endl;
        /*std::cout << "v7 " << v7.x << " " << v7.y << " " << v7.z*/std::cout << uv7.x / scale.x * 0.5 + 1 << " " << uv7.y / scale.z * 0.5 + 1 << std::endl;
        /*std::cout << "v8 " << v8.x << " " << v8.y << " " << v8.z*/std::cout << uv8.x / scale.x * 0.5 + 1 << " " << uv8.y / scale.z * 0.5 + 1 << std::endl;
        auto patch0 = std::array<glm::vec3, 4>{v0, v4, v8, v7};
        auto patch1 = std::array<glm::vec3, 4>{v4, v1, v5, v8};
        auto patch2 = std::array<glm::vec3, 4>{v8, v5, v2, v6};
        auto patch3 = std::array<glm::vec3, 4>{v7, v8, v6, v3};
        tree->Insert(patch0, glm::vec2(v0.x, v0.z), glm::vec2(v8.x, v8.z));
        tree->Insert(patch1, glm::vec2(v4.x, v4.z), glm::vec2(v5.x, v5.z));
        tree->Insert(patch2, glm::vec2(v8.x, v8.z), glm::vec2(v2.x, v2.z));
        tree->Insert(patch3, glm::vec2(v7.x, v7.z), glm::vec2(v6.x, v6.z));
        subdivided = true;
    }
    for (auto i = 0; i < 4; i++) {
        Subdivide(tree->Get(i), texture, scale);
    }
}

std::shared_ptr<Terrain> Terrain::create(const std::string& name,
    glm::vec2 resolution, glm::vec3 scale, std::shared_ptr<Texture> texture)
{
    resolution.x = 10;
    resolution.y = 10;
    auto terrain = std::shared_ptr<Terrain>(new Terrain(name));
    terrain->_terrainData = texture;
    terrain->_terrainResolution = resolution;
    terrain->_terrainSize = scale;
    Mesh::add(terrain);
    Renderable::add(terrain);
    Node::add(terrain);
    /*auto vg = Vgroup::create(name + "vgroup");
    vg->v.resize(uint32_t(resolution.x * resolution.y));
    vg->vn.resize(vg->v.size());
    vg->vt.resize(vg->v.size());*/
    std::vector<glm::vec3>       v;
    std::vector<unsigned>   i;
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    std::cout << scale.x / 2.f << " " << scale.z / 2.f << std::endl;
    Quadtree<std::array<glm::vec3, 4>> quadTree(glm::vec2(-scale.x / 2.f, -scale.z / 2.f), glm::vec2(scale.x / 2.f, scale.z / 2.f));
    for (auto y = 0.f; y < resolution.y; y++) {
        for (auto x = 0.f; x < resolution.x; x++) {
            auto uv = glm::vec2(x / resolution.x, y / resolution.y);
            auto z = 0.f;
            if (texture) {
                glm::vec2    texUV;
                texUV.x = uv.x * texture->size().x;
                texUV.y = uv.y * texture->size().y;
                z = ((float*)texture->texelfetch(texUV))[0];
                
                minZ = z < minZ ? z : minZ;
                maxZ = z > maxZ ? z : maxZ;
            }
            //vg->vt.at(uint32_t(x + y * resolution.x)) = uv;
            //auto &v3 = vg->v.at(uint32_t(x + y * resolution.x));
            auto v3 = glm::vec3(uv.x * scale.x - scale.x / 2.f, z * scale.y, uv.y * scale.z - scale.z / 2.f);
            v.push_back(v3);
            //glm::vec2    pointSize = glm::vec2(scale.x / resolution.x, scale.y / resolution.y);
            if (x < resolution.x - 1 && y < resolution.y - 1) {
                i.push_back(uint32_t(x + y * resolution.x));
                i.push_back(uint32_t(x + (y + 1) * resolution.x));
                i.push_back(uint32_t((x + 1) + (y + 1) * resolution.x));
                i.push_back(uint32_t((x + 1) + y * resolution.x));
            }
        }
    }
    
    for (auto index = 0u; index < i.size() - 4; index += 4) {
        auto i0 = i.at(index + 0);
        auto i1 = i.at(index + 1);
        auto i2 = i.at(index + 2);
        auto i3 = i.at(index + 3);
        auto v0 = v.at(i0);
        auto v1 = v.at(i1);
        auto v2 = v.at(i2);
        auto v3 = v.at(i3);
        auto patch = std::array<glm::vec3, 4>{v0, v1, v2, v3};
        quadTree.Insert(patch, glm::vec2(v0.x, v0.z), glm::vec2(v2.x, v2.z));
        //quadTree.Insert(v1, glm::vec2(v0.x, v0.z), glm::vec2(v2.x, v2.z));
        //quadTree.Insert(v2, glm::vec2(v0.x, v0.z), glm::vec2(v2.x, v2.z));
        //quadTree.Insert(v3, glm::vec2(v0.x, v0.z), glm::vec2(v2.x, v2.z));
    }
    Subdivide(&quadTree, texture, scale);
    /*auto medZ = (maxZ + minZ) / 2.f * scale.y;
    for (auto &v : vg->v)
        v.y -= medZ;*/
    /*
    for (auto index = 0u; index * 3 < vg->i.size(); index++)
    {
        auto i0 = vg->i.at(index * 3 + 0);
        auto i1 = vg->i.at(index * 3 + 1);
        auto i2 = vg->i.at(index * 3 + 2);
        auto v0 = vg->v.at(i0);
        auto v1 = vg->v.at(i1);
        auto v2 = vg->v.at(i2);
        auto &n0 = vg->vn.at(i0);
        auto &n1 = vg->vn.at(i1);
        auto &n2 = vg->vn.at(i2);

        glm::vec3 N0 = glm::vec3((n0.x / 255.f) * 2 - 1, (n0.y / 255.f) * 2 - 1, (n0.z / 255.f) * 2 - 1);
        glm::vec3 N1 = glm::vec3((n1.x / 255.f) * 2 - 1, (n1.y / 255.f) * 2 - 1, (n1.z / 255.f) * 2 - 1);
        glm::vec3 N2 = glm::vec3((n2.x / 255.f) * 2 - 1, (n2.y / 255.f) * 2 - 1, (n2.z / 255.f) * 2 - 1);
        glm::vec3 N;
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
    */
    auto mtl = Material::create("default_terrain");
    //mtl->set_texture_albedo(texture);
    //mtl->set_texture_roughness(texture);
    mtl->albedo = glm::vec3(0.5, 0.5, 0.5);
    mtl->roughness = 0.5;
    //vg->set_material(mtl);
    //terrain->add(vg);
    return terrain;
}

std::shared_ptr<Terrain>  Terrain::create(const std::string& name, glm::vec2 resolution, const std::string &path)
{
    GDALAllRegister();
    auto data = (GDALDataset*)GDALOpen(path.c_str(), GA_ReadOnly );
    if (data == nullptr) {
        throw std::runtime_error(path + " : Could not open");
    }
    auto    band = data->GetRasterBand(1);
    if (band == nullptr) {
        throw std::runtime_error(path + " : Could not Get raster band.");
    }
    double  gt[6];
    auto    err1 = data->GetGeoTransform(gt);
    glm::vec2    size {float(band->GetXSize()), float(band->GetYSize())};
    float   *scanLine = (float *) CPLMalloc(sizeof(float) * size.x * size.y);
    auto    err = band->RasterIO(GF_Read, 0, 0, size.x, size.y,
                      scanLine, size.x, size.y, GDT_Float32,
                      0, 0);
    if (err1 != CE_None || err != CE_None)
    {
        debugLog("Something went wrong with : " + path);
        return Terrain::create(name, resolution, glm::vec3(1, 1, 1),  TextureParser::parse(name + "_terrainData", path));
    }
    std::string proj = data->GetProjectionRef();
    debugLog(proj);
    GDALClose(data);
    auto t = Texture::create(name + "_terrainData", size, GL_TEXTURE_2D,
    GL_RED, GL_COMPRESSED_RED, GL_FLOAT, scanLine);
    CPLFree(scanLine);
    return Terrain::create(name, resolution, glm::vec3(size.x * gt[1], 1, size.y * -gt[5]), t);
}
