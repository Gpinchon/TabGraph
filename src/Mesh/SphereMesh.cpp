/*
* @Author: gpinchon
* @Date:   2020-07-28 00:01:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-07-29 00:09:36
*/

#include "Mesh/SphereMesh.hpp"
#include "Material.hpp"
#include "Mesh/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include <map>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::map<int64_t, unsigned> middlePointIndexCache;

glm::vec2 FindUV(const glm::vec3& normal)
{
    glm::vec2 uv;
    const float& x = normal.x;
    const float& y = normal.y;
    const float& z = normal.z;
    float normalisedX = 0;
    float normalisedZ = -1;
    if (((x * x) + (z * z)) > 0) {
        normalisedX = sqrt((x * x) / ((x * x) + (z * z)));
        if (x < 0) {
            normalisedX = -normalisedX;
        }
        normalisedZ = sqrt((z * z) / ((x * x) + (z * z)));
        if (z < 0) {
            normalisedZ = -normalisedZ;
        }
    }
    if (normalisedZ == 0) {
        uv.x = ((normalisedX * M_PI) / 2);
    } else {
        uv.x = atan(normalisedX / normalisedZ);
        if (normalisedX < 0) {
            uv.x = M_PI - uv.x;
        }
        if (normalisedZ < 0) {
            uv.x += M_PI;
        }
    }
    if (uv.x < 0) {
        uv.x += 2 * M_PI;
    }
    uv.x /= 2 * M_PI;
    uv.y = (-y + 1) / 2;
    return uv;
}

unsigned getMiddlePoint(unsigned p1, unsigned p2, std::vector<glm::vec3>& positions)
{
    // first check if we have it already
    bool firstIsSmaller = p1 < p2;
    int64_t smallerIndex = firstIsSmaller ? p1 : p2;
    int64_t greaterIndex = firstIsSmaller ? p2 : p1;
    auto key = (smallerIndex << 32) + greaterIndex;

    if (middlePointIndexCache.count(key) > 0) {
        return middlePointIndexCache[key];
    }

    // not in cache, calculate it
    glm::vec3 point1 = positions.at(p1);
    glm::vec3 point2 = positions.at(p2);
    glm::vec3 middle = glm::vec3(
        (point1.x + point2.x) / 2.0,
        (point1.y + point2.y) / 2.0,
        (point1.z + point2.z) / 2.0);

    // add vertex makes sure point is on unit sphere
    positions.push_back(normalize(middle));
    auto i = positions.size() - 1;

    // store it, return index
    middlePointIndexCache[key] = i;
    return i;
}

std::shared_ptr<Mesh> SphereMesh::Create(const std::string& name, float radius, unsigned subdivision)
{
    auto m = Mesh::Create(name);
    middlePointIndexCache.clear();
    const float t = (1.0 + std::sqrt(5.0)) / 2.0;

    std::vector<glm::vec3> sphereVertices;
    sphereVertices.push_back(normalize(glm::vec3(-1.0, t, 0.0)));
    sphereVertices.push_back(normalize(glm::vec3(1.0, t, 0.0)));
    sphereVertices.push_back(normalize(glm::vec3(-1.0, -t, 0.0)));
    sphereVertices.push_back(normalize(glm::vec3(1.0, -t, 0.0)));
    sphereVertices.push_back(normalize(glm::vec3(0.0, -1.0, t)));
    sphereVertices.push_back(normalize(glm::vec3(0.0, 1.0, t)));
    sphereVertices.push_back(normalize(glm::vec3(0.0, -1.0, -t)));
    sphereVertices.push_back(normalize(glm::vec3(0.0, 1.0, -t)));
    sphereVertices.push_back(normalize(glm::vec3(t, 0.0, -1.0)));
    sphereVertices.push_back(normalize(glm::vec3(t, 0.0, 1.0)));
    sphereVertices.push_back(normalize(glm::vec3(-t, 0.0, -1.0)));
    sphereVertices.push_back(normalize(glm::vec3(-t, 0.0, 1.0)));

    std::vector<std::array<unsigned, 3>> faces;
    // 5 faces around point 0
    faces.push_back({ 0, 11, 5 });
    faces.push_back({ 0, 5, 1 });
    faces.push_back({ 0, 1, 7 });
    faces.push_back({ 0, 7, 10 });
    faces.push_back({ 0, 10, 11 });

    // 5 adjacent faces
    faces.push_back({ 1, 5, 9 });
    faces.push_back({ 5, 11, 4 });
    faces.push_back({ 11, 10, 2 });
    faces.push_back({ 10, 7, 6 });
    faces.push_back({ 7, 1, 8 });

    // 5 faces around point 3
    faces.push_back({ 3, 9, 4 });
    faces.push_back({ 3, 4, 2 });
    faces.push_back({ 3, 2, 6 });
    faces.push_back({ 3, 6, 8 });
    faces.push_back({ 3, 8, 9 });

    // 5 adjacent faces
    faces.push_back({ 4, 9, 5 });
    faces.push_back({ 2, 4, 11 });
    faces.push_back({ 6, 2, 10 });
    faces.push_back({ 8, 6, 7 });
    faces.push_back({ 9, 8, 1 });
    for (unsigned i = 0; i < subdivision; ++i) {
        std::vector<std::array<unsigned, 3>> faces2;
        for (const auto& tri : faces) {
            // replace triangle by 4 triangles
            auto a = getMiddlePoint(tri[0], tri[1], sphereVertices);
            auto b = getMiddlePoint(tri[1], tri[2], sphereVertices);
            auto c = getMiddlePoint(tri[2], tri[0], sphereVertices);

            faces2.push_back({ tri[0], a, c });
            faces2.push_back({ tri[1], b, a });
            faces2.push_back({ tri[2], c, b });
            faces2.push_back({ a, b, c });
        }
        faces = faces2;
    }
    std::vector<glm::vec3> sphereNormals;
    std::vector<glm::vec2> sphereTexCoords;
    for (auto& v : sphereVertices) {
        sphereNormals.push_back(v);
        sphereTexCoords.push_back(FindUV(v));
        v *= radius;
    }
    std::vector<unsigned> sphereIndices;
    for (const auto& tri : faces) {
        sphereIndices.push_back(tri[0]);
        sphereIndices.push_back(tri[1]);
        sphereIndices.push_back(tri[2]);
    }

    /*for (auto x = 0u; x <= subdivision; ++x) {
        for (auto y = 0u; y <= subdivision; ++y) {
            auto uv = glm::vec2(x / float(subdivision), y / float(subdivision));
            auto v(glm::mix(min, max, glm::vec3(uv.x, 0, uv.y)));
            planeVertices.push_back(v);
            planeNormals.push_back(vn);
            planeTexCoords.push_back(uv);
            if (x < subdivision && y < subdivision) {
                auto a((x + 0) + (y + 0) * (subdivision + 1));
                auto b((x + 1) + (y + 0) * (subdivision + 1));
                auto c((x + 0) + (y + 1) * (subdivision + 1));
                auto d((x + 1) + (y + 1) * (subdivision + 1));
                planeIndices.push_back(a);
                planeIndices.push_back(b);
                planeIndices.push_back(c);
                planeIndices.push_back(b);
                planeIndices.push_back(d);
                planeIndices.push_back(c);
            }
        }
    }*/
    auto vg = Geometry::Create(m->Name() + "_Geometry");
    vg->SetAccessor(Geometry::Position, BufferHelper::CreateAccessor(sphereVertices, GL_ARRAY_BUFFER));
    vg->SetAccessor(Geometry::Normal, BufferHelper::CreateAccessor(sphereNormals, GL_ARRAY_BUFFER, true));
    vg->SetAccessor(Geometry::TexCoord_0, BufferHelper::CreateAccessor(sphereTexCoords, GL_ARRAY_BUFFER));
    vg->SetIndices(BufferHelper::CreateAccessor(sphereIndices, GL_ELEMENT_ARRAY_BUFFER));
    m->AddGeometry(vg);
    m->AddMaterial(Material::Create(m->Name() + "_material"));
    return (m);
}