/*
* @Author: gpinchon
* @Date:   2020-08-16 16:10:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:57
*/

#include "Mesh/Mesh.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Material/Material.hpp"
#include "Assets/Asset.hpp"
#include "Engine.hpp"
#include "Animation/Animation.hpp"

#include "Flame.hpp"
#include "Game.hpp"
#include "Level.hpp"


Flame::Flame()
    : GameEntity("Flame")
    , _spawnTime(std::chrono::high_resolution_clock::now())
{
    _size = glm::vec2(0.7);
    _updateSlot = Engine::OnFixedUpdate().ConnectMember(this, &Flame::_FixedUpdateCPU);
}

Flame::~Flame()
{
    _updateSlot.Disconnect();
}

auto CreateFlameAsset()
{
    auto flameAsset{ Component::Create<Asset>(Engine::ResourcePath() / "models/fire.gltf") };
    flameAsset->Load();
    auto meshes{ flameAsset->GetComponentsInChildren<Mesh>() };
    for (auto& mesh : meshes)
        mesh->Load();
    return meshes;
}

std::shared_ptr<Flame> Flame::Create(const glm::ivec2& position)
{
    auto flame = Component::Create<Flame>();
    static auto flameAsset = CreateFlameAsset();
    for (const auto& mesh : flameAsset)
        flame->AddComponent(mesh);
    flame->SetScale(glm::vec3(0.5));
    Game::CurrentLevel()->SetGameEntityPosition(position, flame);
    return flame;
}

std::chrono::time_point<std::chrono::high_resolution_clock> Flame::SpawnTime() const
{
    return _spawnTime;
}

std::chrono::duration<double> Flame::Timer() const
{
    return _timer;
}

void Flame::SetTimer(std::chrono::duration<double> timer)
{
    _timer = timer;
}

void Flame::_FixedUpdateCPU(float delta)
{
    auto now = std::chrono::high_resolution_clock::now();
    if (now - SpawnTime() > Timer())
        Game::CurrentLevel()->SetGameEntity(Position(), nullptr);
}