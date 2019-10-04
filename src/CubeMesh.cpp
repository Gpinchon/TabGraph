/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-04 16:14:37
*/

#include "CubeMesh.hpp"
#include "Material.hpp" // for Material
#include "Mesh.hpp" // for Mesh
#include "Vgroup.hpp" // for CVEC4, Vgroup
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
    static std::vector<CVEC4> cubeNormals{
        {127, 127, 255, 255},
        {127, 127, 255, 255},
        {127, 127, 255, 255},
        {127, 127, 255, 255},

        {127, 127, 0, 255},
        {127, 127, 0, 255},
        {127, 127, 0, 255},
        {127, 127, 0, 255},

        {0, 127, 127, 255},
        {0, 127, 127, 255},
        {0, 127, 127, 255},
        {0, 127, 127, 255},

        {255, 127, 127, 255},
        {255, 127, 127, 255},
        {255, 127, 127, 255},
        {255, 127, 127, 255},

        {127, 255, 127, 255},
        {127, 255, 127, 255},
        {127, 255, 127, 255},
        {127, 255, 127, 255},

        {127, 0, 127, 255},
        {127, 0, 127, 255},
        {127, 0, 127, 255},
        {127, 0, 127, 255}};
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
    auto vg = Vgroup::Create(m->Name() + "_vgroup");
    auto thisCubeVertices = cubeVertices;
    for (auto &v : thisCubeVertices)
    {
        v *= size;
    }
    vg->set_material(Material::Create(m->Name() + "_material"));
    vg->v = thisCubeVertices;
    vg->vn = cubeNormals;
    vg->vt = cubeTexCoords;
    vg->i = cubeIndices;
    m->AddVgroup(vg);
    return (m);
}
