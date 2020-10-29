/*
* @Author: gpinchon
* @Date:   2020-08-16 19:44:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:25
*/

#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"

#include "CrispyWall.hpp"
#include "Game.hpp"
#include "Level.hpp"

CrispyWall::CrispyWall()
    : GameEntity("CrispyWall")
{
    auto mesh = CubeMesh::Create("WallMesh", glm::vec3(1.f));
    mesh->GetMaterial(0)->SetDiffuse(glm::vec3(0.5, 0.5, 0.5));
    SetComponent(mesh);
}

std::shared_ptr<CrispyWall> CrispyWall::Create()
{
    return tools::make_shared<CrispyWall>();
}

void CrispyWall::Die()
{
    Game::CurrentLevel()->SetGameEntity(Position(), nullptr);
}