/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:13:43
*/

#include "Mesh/Mesh.hpp" // for Mesh
#include "Mesh/Geometry.hpp" // for CVEC4, Geometry
#include "Mesh/CubeMesh.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Material.hpp" // for Material
#include <vector> // for vector

std::shared_ptr<Mesh> CubeMesh::Create(const std::string &name, glm::vec3 size)
{
    auto m = Mesh::Create(name);
    static std::vector<glm::vec3> cubeVertices{
        {-0.50f, -0.50f, 0.50f}, // back
        {0.50f, -0.50f, 0.50f},
        {0.50f, 0.50f, 0.50f},
        {-0.50f, 0.50f, 0.50f},

        {-0.50f, -0.50f, -0.50f}, // front
        {0.50f, -0.50f, -0.50f},
        {0.50f, 0.50f, -0.50f},
        {-0.50f, 0.50f, -0.50f},

        {-0.50f, -0.50f, -0.50f}, // left
        {-0.50f, -0.50f, 0.50f},
        {-0.50f, 0.50f, 0.50f},
        {-0.50f, 0.50f, -0.50f},

        {0.50f, -0.50f, -0.50f}, // right
        {0.50f, -0.50f, 0.50f},
        {0.50f, 0.50f, 0.50f},
        {0.50f, 0.50f, -0.50f},

        {-0.50f, 0.50f, 0.50f}, // top
        {0.50f, 0.50f, 0.50f},
        {0.50f, 0.50f, -0.50f},
        {-0.50f, 0.50f, -0.50f},

        {-0.50f, -0.50f, 0.50f}, // bottom
        {0.50f, -0.50f, 0.50f},
        {0.50f, -0.50f, -0.50f},
        {-0.50f, -0.50f, -0.50f}};
    static std::vector<glm::vec3> cubeNormals{
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},

        {0, 0, -1},
        {0, 0, -1},
        {0, 0, -1},
        {0, 0, -1},

        {-1, 0, 0},
        {-1, 0, 0},
        {-1, 0, 0},
        {-1, 0, 0},

        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},

        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},

        {0, -1, 0},
        {0, -1, 0},
        {0, -1, 0},
        {0, -1, 0}};
    static std::vector<glm::vec2> cubeTexCoords{
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        {1, 0},
        {0, 0},
        {0, 1},
        {1, 1},
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        {1, 0},
        {0, 0},
        {0, 1},
        {1, 1},
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        {1, 0},
        {0, 0},
        {0, 1},
        {1, 1}};
    static std::vector<unsigned> cubeIndices{
        0, 1, 2, 0, 2, 3, // front
        4, 6, 5, 4, 7, 6, // back
        8, 9, 10, 8, 10, 11, // left
        12, 14, 13, 12, 15, 14, // right
        16, 17, 18, 16, 18, 19, // top
        20, 22, 21, 20, 23, 22 // bottom
    };
    auto vg = Geometry::Create(m->Name() + "_Geometry");
    auto thisCubeVertices = cubeVertices;
    for (auto &v : thisCubeVertices)
    {
        v *= size;
    }
    vg->SetAccessor(Geometry::Position, BufferHelper::CreateAccessor(thisCubeVertices, GL_ARRAY_BUFFER));
    vg->SetAccessor(Geometry::Normal, BufferHelper::CreateAccessor(cubeNormals, GL_ARRAY_BUFFER, true));
    vg->SetAccessor(Geometry::TexCoord_0, BufferHelper::CreateAccessor(cubeTexCoords, GL_ARRAY_BUFFER));
    vg->SetIndices(BufferHelper::CreateAccessor(cubeIndices, GL_ELEMENT_ARRAY_BUFFER));
    m->AddGeometry(vg);
    m->AddMaterial(Material::Create(m->Name() + "_material"));
    return (m);
}