/*
* @Author: gpinchon
* @Date:   2020-08-08 22:47:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:36
*/

#include "Engine.hpp"
#include "Animation/Animation.hpp"
#include "Assets/Asset.hpp"
#include "Camera/Camera.hpp"
#include "Event/Keyboard.hpp"
#include "Material/Material.hpp"
#include "Mesh/CapsuleMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Scene.hpp"

#include "Bomb.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

#define DOWNK SDL_SCANCODE_DOWN
#define UPK SDL_SCANCODE_UP
#define LEFTK SDL_SCANCODE_LEFT
#define RIGHTK SDL_SCANCODE_RIGHT
#define ZOOMK SDL_SCANCODE_KP_PLUS
#define UNZOOMK SDL_SCANCODE_KP_MINUS

Player::Player(const std::string& name, const glm::vec3& color)
    : GameEntity(name, "Player")
{
    _height = 0;
    Engine::OnFixedUpdate().ConnectMember(this, &Player::_FixedUpdateCPU);
    Keyboard::OnKeyDown(SDL_SCANCODE_SPACE).ConnectMember(this, &Player::DropBomb);
}

auto CreatePlayerAsset()
{
    auto playerAsset = Component::Create<Asset>(Engine::ResourcePath() / "models/bomberman.gltf");
    playerAsset->Load();
    playerAsset->GetComponent<Scene>()->GetComponent<Node>()->SetScale(glm::vec3(0.01f));
    return playerAsset;
}

std::shared_ptr<Player> Player::Create(const glm::vec3& color)
{
    auto player = Component::Create<Player>("Player1", glm::vec3(1));
    static auto playerAsset = CreatePlayerAsset();
    auto playerAssetClone = playerAsset->GetComponent<Scene>()->Clone();
    auto playerNode = playerAssetClone->GetComponent<Node>();
    player->AddChild(playerNode);
    for (auto& animation : playerAssetClone->GetComponents<Animation>())
        player->AddAnimation(animation);
    player->PlayAnimation("idle", true);
    player->GetComponentInChildrenByName<Material>("White")->SetDiffuse(color);
    return player;
}

float Player::Speed() const
{
    return _speed;
}

void Player::SetSpeed(float speed)
{
    _speed = speed;
}

void Player::DropBomb(const SDL_KeyboardEvent& event) const
{
    if (event.type != SDL_KEYUP && !event.repeat && Game::CurrentLevel()->GetGameEntity(Position()) == nullptr) {
        std::cout << Position().x << ' ' << Position().y << std::endl;
        Bomb::Create(Position());
    } else if (Game::CurrentLevel()->GetGameEntity(Position()) != nullptr)
        std::cout << Game::CurrentLevel()->GetGameEntity(Position())->Type() << std::endl;
}

template <typename T>
auto ProjectPointOnPlane(const T& point, const T& planePosition, const T& planeNormal)
{
    auto pointToPlanePosition = point - planePosition;
    auto dist = dot(planeNormal, pointToPlanePosition);
    return point - dist * planeNormal;
}

struct Contact {
    glm::vec2 normal;
    float penetration;
};

auto AABBvsCircle(Contact& c, const glm::vec2& sphereCenter, float sphereRadius, const glm::vec2& boxPos, const glm::vec2& boxSize)
{
    /*glm::vec2 aabbSize((boxMax - boxMin) / 2.f);
    glm::vec2 nearest = glm::max(boxPos - aabbSize, glm::min(sphereCenter, boxPos + aabbSize));
    glm::vec2 delta = sphereCenter - nearest;
    c.normal = -normalize(delta);
    c.penetration = sphereRadius - length(delta);
    return (delta.x * delta.x + delta.y * delta.y) < (sphereRadius * sphereRadius);*/
    //auto NearestX = glm::max(boxPos.x, glm::min(sphereCenter.x, boxPos.x + aabbSize.x));
    //auto NearestY = glm::max(boxPos.y, glm::min(sphereCenter.y, boxPos.y + aabbSize.y));
    /*auto DeltaX = sphereCenter.x - NearestX;
    auto DeltaY = sphereCenter.y - NearestY;
    return (DeltaX * DeltaX + DeltaY * DeltaY) < (sphereRadius * sphereRadius);*/
    //glm::vec2 aabb_half_extents(0.5f);
    auto C = sphereCenter;
    auto B = boxPos;
    auto BC = C - B; //D
    auto P = B + glm::clamp(BC, -boxSize, boxSize);
    auto CP = P - C;
    auto CPLen = length(CP);
    auto CPdBC = dot(CP, BC);
    if (CPLen < sphereRadius) {
        if (CPLen == 0.f)
            c.normal = -normalize(BC);
        else
            c.normal = CP / CPLen;
        c.penetration = sphereRadius - CPLen;
        return true;
    }
    return false;
}

void Player::Move(const glm::vec2& direction, float delta)
{
    auto size = 0.5f;
    auto dirLength = length(direction);
    if (dirLength == 0.f)
        return;
    auto dir = direction / dirLength;
    auto axis = dir * Speed() * delta;
    auto newPlayerPosition = Position() + axis;
    //Game::CurrentLevel()->SetGameEntity(Position(), nullptr);
    LookAt(Position() - direction);
    SetPosition(newPlayerPosition);
    PlayAnimation("run", true);
    //Game::CurrentLevel()->SetGameEntity(newPlayerPosition, std::static_pointer_cast<Player>(shared_from_this()));
}

#include "Renderer/Renderer.hpp"

void Player::Die()
{
    _lives--;
    if (GetComponentByName<Animation>("death")->Playing()) {
        std::cout << "Already DED" << std::endl;
    }
    PlayAnimation("death", false);

    std::cout << "DED on Frame "<< Renderer::FrameNumber() << std::endl;
}

void Player::_FixedUpdateCPU(float delta)
{
    if (Game::CurrentLevel() == nullptr ||
        GetComponentByName<Animation>("death")->Playing())
        return;
    glm::vec2 input;
    input.x = Keyboard::key(UPK) - Keyboard::key(DOWNK);
    input.y = Keyboard::key(RIGHTK) - Keyboard::key(LEFTK);
    auto inputLength = length(input);
    if (inputLength > 0.f) {
        input /= inputLength;
        auto cameraT = Game::CurrentLevel()->CurrentCamera();
        auto cameraPosition = glm::vec2(cameraT->WorldPosition().x, cameraT->WorldPosition().z);
        auto cameraForward = normalize(glm::vec2(cameraT->Forward().x, cameraT->Forward().z));
        auto projPlayerPosition = ProjectPointOnPlane(Position(), cameraPosition, cameraForward);
        auto forward = normalize(Position() - projPlayerPosition);
        auto right = normalize(glm::vec2(cameraT->Right().x, cameraT->Right().z));
        Move(input * (forward + right), delta);
    }
    else
        PlayAnimation("idle", true);
    bool collides;
    do {
        collides = false;
        auto maxX = glm::clamp(int(Position().x + 1), 0, Game::CurrentLevel()->Size().x - 1);
        auto minX = glm::clamp(int(Position().x - 1), 0, Game::CurrentLevel()->Size().x - 1);
        auto maxY = glm::clamp(int(Position().y + 1), 0, Game::CurrentLevel()->Size().y - 1);
        auto minY = glm::clamp(int(Position().y - 1), 0, Game::CurrentLevel()->Size().y - 1);
        for (auto x = minX; x <= maxX; ++x) {
            for (auto y = minY; y <= maxY; ++y) {
                auto entity = Game::CurrentLevel()->GetGameEntity(glm::ivec2(x, y));
                if (entity == nullptr) {
                    continue;
                }
                if (x == int(Position().x) && y == int(Position().y) && entity->Type() == "Bomb")
                    continue;
                Contact contact;
                auto intersects = AABBvsCircle(contact, Position(), 0.3f, entity->Position(), entity->Size() * 0.5f);
                if (intersects) {
                    if (entity->Type() == "Flame") {
                        Die();
                        return;
                    }
                    SetPosition(Position() - contact.normal * std::max(contact.penetration, 0.001f));
                    collides = true;
                }
            }
        }
    } while (collides);
}