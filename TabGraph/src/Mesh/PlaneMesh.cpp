/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-09 17:56:48
*/

#include "Mesh/PlaneMesh.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Material/Material.hpp" // for Material
#include "Mesh/Geometry.hpp" // for CVEC4, Geometry
#include "Mesh/Mesh.hpp" // for Mesh
#include <vector> // for vector

std::shared_ptr<Mesh> PlaneMesh::Create(const std::string& name, glm::vec2 size, unsigned subdivision)
{
    auto m = Mesh::Create(name);
    glm::vec3 maxV = glm::vec3(size.x / 2.f, 0.f, size.y / 2.f);
    glm::vec3 minV = -maxV;
    glm::vec3 vn(0, 1, 0);
    std::vector<glm::vec3> planeVertices;
    std::vector<glm::vec3> planeNormals;
    std::vector<glm::vec2> planeTexCoords;
    std::vector<unsigned> planeIndices;
    for (auto x = 0u; x <= subdivision; ++x) {
        for (auto y = 0u; y <= subdivision; ++y) {
            auto uv = glm::vec2(x / float(subdivision), y / float(subdivision));
            auto v(glm::mix(minV, maxV, glm::vec3(uv.x, 0, uv.y)));
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
    }
    auto vg = Geometry::Create(m->Name() + "_Geometry");
    vg->SetAccessor(Geometry::AccessorKey::Position, BufferHelper::CreateAccessor(planeVertices, GL_ARRAY_BUFFER));
    vg->SetAccessor(Geometry::AccessorKey::Normal, BufferHelper::CreateAccessor(planeNormals, GL_ARRAY_BUFFER, true));
    vg->SetAccessor(Geometry::AccessorKey::TexCoord_0, BufferHelper::CreateAccessor(planeTexCoords, GL_ARRAY_BUFFER));
    vg->SetIndices(BufferHelper::CreateAccessor(planeIndices, GL_ELEMENT_ARRAY_BUFFER));
    m->AddGeometry(vg);
    m->AddMaterial(Material::Create(m->Name() + "_material"));
    return (m);
}
