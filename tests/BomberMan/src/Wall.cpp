/*
* @Author: gpinchon
* @Date:   2020-08-09 22:51:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:45
*/
#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Assets/Asset.hpp"
#include "Engine.hpp"
#include "Scene/Scene.hpp"

#include "Wall.hpp"

Wall::Wall()
    : GameEntity("Wall")
{
    
}

auto CreateWallAsset() {
    auto wall(Component::Create<Wall>());
    auto wallAsset{ Component::Create<Asset>(Engine::ResourcePath() / "models/wall.gltf") };
    wallAsset->Load();
    auto wallMeshes = wallAsset->GetComponentsInChildren<Mesh>();
    for (const auto& mesh : wallMeshes)
        wall->AddComponent(mesh);
    wall->SetScale(glm::vec3(0.45));
    return wall;
}

std::shared_ptr<Wall> Wall::Create()
{
    static auto wallAsset(CreateWallAsset());
    auto wall{ std::static_pointer_cast<Wall>(wallAsset->Clone()) };
    return wall;
}