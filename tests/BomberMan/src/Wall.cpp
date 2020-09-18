/*
* @Author: gpinchon
* @Date:   2020-08-09 22:51:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:45
*/
#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"

#include "Wall.hpp"

Wall::Wall()
    : GameEntity("Wall")
{
    auto mesh = CubeMesh::Create("WallMesh", glm::vec3(1.f));
    mesh->GetMaterial(0)->SetAlbedo(glm::vec3(1, 0.2, 0.2));
    SetComponent(mesh);
}

std::shared_ptr<Wall> Wall::Create()
{
    std::shared_ptr<Wall> wall(new Wall);
    return wall;
}