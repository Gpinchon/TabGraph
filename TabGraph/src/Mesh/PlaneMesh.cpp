/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:18
*/

#include "Mesh/PlaneMesh.hpp"
#include "Material/Material.hpp" // for Material
#include "Mesh/Geometry.hpp" // for CVEC4, Geometry
#include "Mesh/Mesh.hpp" // for Mesh
#include <vector> // for vector

std::shared_ptr<Mesh> PlaneMesh::Create(const std::string& name, glm::vec2 size, unsigned subdivision)
{
    auto m = Component::Create<Mesh>(name);
    glm::vec3 maxV = glm::vec3(size.x / 2.f, 0.f, size.y / 2.f);
    glm::vec3 minV = -maxV;
    glm::vec3 vn(0, 1, 0);
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoord;
    std::vector<unsigned> indices;
    //auto vertexBuffer{ Component::Create<Buffer>(0) };
    //auto indiceBuffer{ Component::Create<Buffer>(0) };
    for (auto x = 0u; x <= subdivision; ++x) {
        for (auto y = 0u; y <= subdivision; ++y) {
            auto uv = glm::vec2(x / float(subdivision), y / float(subdivision));
            auto v(glm::mix(minV, maxV, glm::vec3(uv.x, 0, uv.y)));
            vertices.push_back(v);
            normals.push_back(vn);
            texCoord.push_back(uv);
            if (x < subdivision && y < subdivision) {
                auto a((x + 0) + (y + 0) * (subdivision + 1));
                auto b((x + 1) + (y + 0) * (subdivision + 1));
                auto c((x + 0) + (y + 1) * (subdivision + 1));
                auto d((x + 1) + (y + 1) * (subdivision + 1));
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }
    //auto vertexBufferView{ Component::Create<BufferView>(vertexBuffer) };
    //auto indiceBufferView{ Component::Create<BufferView>(vertexBuffer) };
    //vertexBufferView->SetByteStride(sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2));

    //auto vertexAccessor{ Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Float32 , BufferAccessor::DataType::Vec3, vertexBufferView) };
    //auto normalAccessor{ Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Float32 , BufferAccessor::DataType::Vec3, vertexBufferView) };
    //auto texcoordAccessor{ Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Float32 , BufferAccessor::DataType::Vec2, vertexBufferView) };
    //normalAccessor->SetByteOffset(sizeof(glm::vec3));
    //normalAccessor->SetNormalized(true);
    //texcoordAccessor->SetByteOffset(sizeof(glm::vec3) + sizeof(glm::vec3));

    //auto indiceAccessor{ Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Uint32 , BufferAccessor::DataType::Scalar, indiceBufferView) };

    //auto vg = Component::Create<Geometry>(m->GetName() + "_Geometry");
    //vg->SetAccessor(Geometry::AccessorKey::Position, vertexAccessor);//BufferHelper::CreateAccessor(planeVertices, GL_ARRAY_BUFFER));
    //vg->SetAccessor(Geometry::AccessorKey::Normal, normalAccessor);
    //vg->SetAccessor(Geometry::AccessorKey::TexCoord_0, texcoordAccessor);
    //vg->SetIndices(indiceAccessor);
    auto vg{ Component::Create<Geometry>(vertices, normals, texCoord, indices) };
    m->AddGeometry(vg);
    m->AddMaterial(Component::Create<Material>(m->GetName() + "_material"));
    return (m);
}
