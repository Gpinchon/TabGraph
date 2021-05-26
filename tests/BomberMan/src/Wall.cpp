/*
* @Author: gpinchon
* @Date:   2020-08-09 22:51:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:45
*/
#include "Material/Material.hpp"
#include "Surface/CubeMesh.hpp"
#include "Surface/Mesh.hpp"
#include "Assets/Asset.hpp"
#include "Engine.hpp"
#include "Scene/Scene.hpp"

#include "Wall.hpp"

Wall::Wall(Level& level)
    : GameEntity(level, "Wall")
{
    
}

auto CreateWallAsset() {
    auto wallAsset{ Component::Create<Asset>(Engine::GetResourcePath() / "models/wall.gltf") };
    wallAsset->Load();
    return wallAsset->GetComponentsInChildren<Mesh>();
    /*for (const auto& mesh : wallMeshes)
        wall->AddComponent(mesh);
    wall->SetScale(glm::vec3(0.45));
    return wall;*/
}

std::shared_ptr<Wall> Wall::Create(Level& level)
{
    static auto wallMeshes(CreateWallAsset());
    //auto wall{ std::static_pointer_cast<Wall>(wallAsset->Clone()) };
    auto wall(Component::Create<Wall>(level));
    for (const auto& mesh : wallMeshes)
        wall->AddComponent(mesh);
    wall->SetScale(glm::vec3(0.45));
    return wall;
}