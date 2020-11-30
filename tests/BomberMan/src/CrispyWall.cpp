/*
* @Author: gpinchon
* @Date:   2020-08-16 19:44:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:25
*/

#include "Mesh/Mesh.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Material/Material.hpp"
#include "Assets/AssetsParser.hpp"
#include "Engine.hpp"
#include "Transform.hpp"
#include "Animation/Animation.hpp"

#include "CrispyWall.hpp"
#include "Game.hpp"
#include "Level.hpp"

CrispyWall::CrispyWall()
    : GameEntity("CrispyWall")
{
    _height = 0;
}

std::shared_ptr<CrispyWall> CrispyWall::Create()
{
    auto wall = Component::Create<CrispyWall>();
    static auto crispyWallMeshes = AssetsParser::Parse(Engine::ResourcePath() / "models/crispyWall/crispyWall.gltf")->GetComponentsInChildren<Mesh>();
    for (const auto& mesh : crispyWallMeshes)
        wall->AddComponent(mesh);
    //wall->AddChild(std::static_pointer_cast<Node>(crispyWallMesh->Clone()));
    wall->SetScale(glm::vec3(0.01));
    return wall;
}

void CrispyWall::Die()
{
    Game::CurrentLevel()->SetGameEntity(Position(), nullptr);
}