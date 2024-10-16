/*
* @Author: gpinchon
* @Date:   2020-08-16 19:44:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:25
*/

#include <Surface/Mesh.hpp>
#include <Surface/SphereMesh.hpp>
#include <Material/Material.hpp>
#include <Assets/Asset.hpp>
#include <Engine.hpp>
#include <Animation/Animation.hpp>
#include <Assets/AssetsParser.hpp>

#include "CrispyWall.hpp"
#include "Game.hpp"
#include "Level.hpp"

CrispyWall::CrispyWall(Level& level)
    : GameEntity(level, "CrispyWall")
{
    _height = 0;
}

auto CreateCrispyWallAsset() {
    
    auto crispyWallAsset{ Component::Create<Asset>(Engine::GetResourcePath() / "models/crispyWall.gltf") };
    AssetsParser::AddParsingTask({
        AssetsParser::ParsingTask::Type::Sync,
        crispyWallAsset
    });
    auto crispyWallMeshes{ crispyWallAsset->GetComponentsInChildren<Mesh>() };
    for (const auto& mesh : crispyWallMeshes)
        mesh->Load();
    return crispyWallMeshes;
}

std::shared_ptr<CrispyWall> CrispyWall::Create(Level& level)
{
    auto wall = Component::Create<CrispyWall>(level);
    static auto crispyWallAsset{ CreateCrispyWallAsset() };
    for (auto mesh : crispyWallAsset)
        wall->AddComponent<Surface>(mesh);
    return wall;
}

void CrispyWall::Die()
{
    _level.SetGameEntity(Position(), nullptr);
}