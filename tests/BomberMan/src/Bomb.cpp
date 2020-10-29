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

std::shared_ptr<Bomb> Bomb::Create(const glm::ivec2& position)
{
    auto bomb = tools::make_shared<Bomb>();
    static auto bombAsset = AssetsParser::Parse(Engine::ResourcePath() / "models/bomb/scene.gltf");
    auto bombAssetClone = bombAsset->Clone();
    auto bombNode = bombAssetClone->GetComponent<Scene>()->RootNodes().at(0);
    bombNode->SetScale(glm::vec3(0.4f));
    bomb->AddChild(bombNode);
    //bombNode->SetParent(bomb);
    for (auto animation : bombAssetClone->GetComponents<Animation>())
        bomb->AddAnimation(animation);
    bomb->PlayAnimation("Armature|idle", true);
    Game::CurrentLevel()->SetGameEntityPosition(position, bomb);
    //Keyboard::AddKeyCallback(SDL_SCANCODE_SPACE, Callback<void(const SDL_KeyboardEvent&)>::Create(&Player::DropBomb, player, std::placeholders::_1));
    return bomb;
    /*auto bomb = std::shared_ptr<Bomb>(new Bomb);
    static auto asset = AssetsParser::Parse(Engine::ResourcePath() / "models/bomb/scene.gltf");
    auto node = asset.GetComponent<Scene>()->RootNodes().at(0);
    node->GetComponent<Transform>()->SetScale(glm::vec3(0.01f));
    node->SetParent(bomb);
    for (auto animation : asset.GetComponents<Animation>())
        bomb->AddAnimation(animation);
    bomb->PlayAnimation("Armature|idle", true);
    Game::CurrentLevel()->SetGameEntityPosition(position, bomb);
    return bomb;*/
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
    auto now = std::chrono::high_resolution_clock::now();
    if (now - SpawnTime() > Timer())
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
