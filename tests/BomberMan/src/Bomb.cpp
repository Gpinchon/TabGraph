/*
* @Author: gpinchon
* @Date:   2020-08-15 16:14:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:52:03
*/

#include "Mesh/Mesh.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Material/Material.hpp"
#include "Assets/AssetsParser.hpp"
#include "Engine.hpp"
#include "Transform.hpp"
#include "Animation/Animation.hpp"

#include <chrono>

#include "Bomb.hpp"
#include "Flame.hpp"
#include "Game.hpp"
#include "Level.hpp"

Bomb::Bomb()
    : GameEntity("Bomb")
    , _spawnTime(std::chrono::high_resolution_clock::now())
{
    _height = 0;
}

Bomb::Bomb(const Bomb& bomb) : GameEntity(bomb)
    , _range(bomb._range)
    , _timer(bomb._timer)
    , _spawnTime(std::chrono::high_resolution_clock::now())
    , _bombMaterial(GetComponentInChildrenByName<Material>("Body"))
{
}

auto CreateBomb() {
    auto bomb = Component::Create<Bomb>();
    auto bombAsset = AssetsParser::Parse(Engine::ResourcePath() / "models/bomb/bomb.gltf");
    auto bombNode = bombAsset->GetComponentByName<Node>("Bomb");
    //for (auto& node : bombAsset->GetComponents<Node>())
    //    bomb->AddComponent(node);
    //bombNode->SetParent(bomb);
    //bombNode->SetScale(glm::vec3(0.4f));
    bomb->AddChild(bombNode);
    for (auto &animation : bombAsset->GetComponents<Animation>())
        bomb->AddAnimation(animation);
    bomb->PlayAnimation("idle", true);
    return bomb;
}

std::shared_ptr<Bomb> Bomb::Create(const glm::ivec2& position)
{
    static auto bomb = CreateBomb();
    auto bombClone = std::static_pointer_cast<Bomb>(bomb->Clone());
    bombClone->_bombMaterial = bombClone->GetComponentInChildrenByName<Material>("Body");
    bombClone->_spawnTime = std::chrono::high_resolution_clock::now();
    Game::CurrentLevel()->SetGameEntityPosition(position, bombClone);
    return bombClone;
}

void SpawnFlames(const glm::ivec2& position, const glm::ivec2& direction, int range)
{
    auto level = Game::CurrentLevel();
    for (auto i = 1; i < range; ++i) {
        auto flamePosition = position + direction * i;
        if (flamePosition.x >= 0 && flamePosition.x < level->Size().x
            && flamePosition.y >= 0 && flamePosition.y < level->Size().y) {
            if (level->GetGameEntity(flamePosition) == nullptr || level->GetGameEntity(flamePosition)->Type() == "Flame")
                Flame::Create(flamePosition);
            else {
                if (level->GetGameEntity(flamePosition)->Type() == "Bomb") {
                    auto bomb = std::static_pointer_cast<Bomb>(level->GetGameEntity(flamePosition));
                    bomb->ResetTimer();
                    bomb->SetTimer(std::chrono::duration<double>(0.01));
                    //level->GetGameEntity(flamePosition)->Die();
                } else if (level->GetGameEntity(flamePosition)->Type() == "CrispyWall") {
                    level->GetGameEntity(flamePosition)->Die();
                    Flame::Create(flamePosition);
                }
                break;
            }
        }
    }
}

void Bomb::ResetTimer()
{
    _spawnTime = std::chrono::high_resolution_clock::now();
}

void Bomb::Die()
{
    auto position = glm::ivec2(Position());

    Flame::Create(position);
    SpawnFlames(position, glm::ivec2(1, 0), Range());
    SpawnFlames(position, glm::ivec2(-1, 0), Range());
    SpawnFlames(position, glm::ivec2(0, 1), Range());
    SpawnFlames(position, glm::ivec2(0, -1), Range());
}

void Bomb::_FixedUpdateCPU(float delta)
{
    auto deltaTime = std::chrono::high_resolution_clock::now() - SpawnTime();
    _bombMaterial->SetEmissive(glm::mix(glm::vec3(0), glm::vec3(0.3, 0, 0), deltaTime / Timer()));
    if (deltaTime > Timer())
        Die();
}

std::chrono::time_point<std::chrono::high_resolution_clock> Bomb::SpawnTime() const
{
    return _spawnTime;
}

std::chrono::duration<double> Bomb::Timer() const
{
    return _timer;
}

void Bomb::SetTimer(std::chrono::duration<double> timer)
{
    _timer = timer;
}

int Bomb::Range() const
{
    return _range;
}

void Bomb::SetRange(int range)
{
    _range = range;
}
