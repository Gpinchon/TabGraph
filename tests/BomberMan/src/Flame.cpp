/*
* @Author: gpinchon
* @Date:   2020-08-16 16:10:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:54:57
*/

#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"

#include "Flame.hpp"
#include "Game.hpp"
#include "Level.hpp"

Flame::Flame()
    : GameEntity("Flame")
    , _spawnTime(std::chrono::high_resolution_clock::now())
{
    auto mesh = CubeMesh::Create("FlameMesh", glm::vec3(0.5f));
    mesh->GetMaterial(0)->SetDiffuse(glm::vec3(0.886274f, 0.345098f, 0.133333f));
    mesh->GetMaterial(0)->SetEmissive(glm::vec3(0.886274f, 0.345098f, 0.133333f));
    SetComponent(mesh);
}

std::shared_ptr<Flame> Flame::Create(const glm::ivec2& position)
{
    auto flame = tools::make_shared<Flame>();
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
    //Explode();
}