/*
* @Author: gpinchon
* @Date:   2020-08-16 19:44:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:25
*/

#include "Mesh/Mesh.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Material/Material.hpp"
#include "Assets/Asset.hpp"
#include "Engine.hpp"
#include "Animation/Animation.hpp"

#include "CrispyWall.hpp"
#include "Game.hpp"
#include "Level.hpp"

CrispyWall::CrispyWall()
    : GameEntity("CrispyWall")
{
    _height = 0;
}

auto CreateCrispyWallAsset() {
    auto wall = Component::Create<CrispyWall>();
    auto bombAsset{ Component::Create<Asset>(Engine::ResourcePath() / "models/crispyWall/crispyWall.gltf") };
    bombAsset->Load();
    auto crispyWallMeshes = bombAsset->GetComponentsInChildren<Mesh>();
    for (const auto& mesh : crispyWallMeshes)
        wall->AddComponent(mesh);
    return wall;
}

std::shared_ptr<CrispyWall> CrispyWall::Create()
{
    static auto crispyWallAsset{ CreateCrispyWallAsset() };
    auto wall{ std::static_pointer_cast<CrispyWall>(crispyWallAsset->Clone()) };
    wall->SetScale(glm::vec3(0.01));
    return wall;
}

void CrispyWall::Die()
{
    Game::CurrentLevel()->SetGameEntity(Position(), nullptr);
}