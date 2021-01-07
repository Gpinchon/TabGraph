/*
* @Author: gpinchon
* @Date:   2020-08-16 16:10:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:57
*/

#include "Mesh/Mesh.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Material/Material.hpp"
#include "Assets/AssetsParser.hpp"
#include "Engine.hpp"
#include "Transform.hpp"
#include "Animation/Animation.hpp"

#include "Flame.hpp"
#include "Game.hpp"
#include "Level.hpp"


Flame::Flame()
    : GameEntity("Flame")
    , _spawnTime(std::chrono::high_resolution_clock::now())
{
    _size = glm::vec2(0.7);
    Engine::OnFixedUpdate().ConnectMember(this, &Flame::_FixedUpdateCPU);
}

auto CreateFlame() {
    auto flame = Component::Create<Flame>();
    static auto flameAsset = AssetsParser::Parse(Engine::ResourcePath() / "models/fire/fire.gltf");
    static auto flameMeshes = flameAsset->GetComponentsInChildren<Mesh>();
    for (const auto& mesh : flameMeshes)
        flame->AddComponent(mesh);
    return flame;
}

std::shared_ptr<Flame> Flame::Create(const glm::ivec2& position)
{
    auto flame = CreateFlame();
    flame->SetScale(glm::vec3(0.5));
    //auto flameClone = std::static_pointer_cast<Flame>(flame->Clone());
    Game::CurrentLevel()->SetGameEntityPosition(position, flame);
    //return flameClone;
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