/*
* @Author: gpinchon
* @Date:   2020-08-08 22:47:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 14:00:36
*/

#include "Camera/Camera.hpp"
#include "Input/Keyboard.hpp"
#include "Material.hpp"
#include "Mesh/CapsuleMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Transform.hpp"

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
    auto playerMesh = CapsuleMesh::Create("PlayerMesh", 0.5, 0.4f, 5);
    playerMesh->GetMaterial(0)->SetAlbedo(color);
    SetComponent(playerMesh);
    Keyboard::AddKeyCallback(SDL_SCANCODE_SPACE, Callback<void(const SDL_KeyboardEvent&)>::Create(&Player::DropBomb, this, std::placeholders::_1));
}

std::shared_ptr<Player> Player::Create(const glm::vec3& color)
{
    std::shared_ptr<Player> player(new Player("Player1", color));
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

auto AABBvsCircle(Contact& c, const glm::vec2& sphereCenter, float sphereRadius, const glm::vec2& boxPos, const glm::vec2& boxMin, const glm::vec2& boxMax)
{
    glm::vec2 aabb_half_extents((boxMax - boxMin) / 2.f);
    auto C = sphereCenter;
    auto B = boxPos;
    auto BC = C - B; //D
    auto P = B + glm::clamp(BC, -aabb_half_extents, aabb_half_extents);
    auto CP = P - C;
    auto CPLen = length(CP);
    if (CPLen <= sphereRadius) {
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
    LookAt(Position() + direction);
    SetPosition(newPlayerPosition);
    //Game::CurrentLevel()->SetGameEntity(newPlayerPosition, std::static_pointer_cast<Player>(shared_from_this()));
}

void Player::Die()
{
    std::cout << "DED" << std::endl;
}

void Player::_FixedUpdateCPU(float delta)
{
    if (Game::CurrentLevel() == nullptr)
        return;
    glm::vec2 input;
    input.x = Keyboard::key(UPK) - Keyboard::key(DOWNK);
    input.y = Keyboard::key(RIGHTK) - Keyboard::key(LEFTK);
    if (length(input) > 0.f) {
        auto cameraT = Game::CurrentLevel()->CurrentCamera()->GetComponent<Transform>();
        auto cameraPosition = glm::vec2(cameraT->WorldPosition().x, cameraT->WorldPosition().z);
        auto cameraForward = normalize(glm::vec2(cameraT->Forward().x, cameraT->Forward().z));
        auto projPlayerPosition = ProjectPointOnPlane(Position(), cameraPosition, cameraForward);
        auto forward = normalize(Position() - projPlayerPosition);
        auto right = glm::vec2(cameraT->Right().x, cameraT->Right().z);
        Move(input * (forward + right), delta);
    }
    auto maxX = glm::clamp(int(Position().x + 1), 0, Game::CurrentLevel()->Size().x - 1);
    auto minX = glm::clamp(int(Position().x - 1), 0, Game::CurrentLevel()->Size().x - 1);
    auto maxY = glm::clamp(int(Position().y + 1), 0, Game::CurrentLevel()->Size().y - 1);
    auto minY = glm::clamp(int(Position().y - 1), 0, Game::CurrentLevel()->Size().y - 1);
    bool foundNewDir = false;
    for (auto x = minX; x <= maxX; ++x) {
        for (auto y = minY; y <= maxY; ++y) {
            auto entity = Game::CurrentLevel()->GetGameEntity(glm::ivec2(x, y));
            if (entity == nullptr) {
                continue;
            }
            if (x == int(Position().x) && y == int(Position().y) && entity->Type() == "Bomb")
                continue;
            Contact contact;
            auto intersects = AABBvsCircle(contact, Position(), 0.4f, entity->Position(), entity->Position() - 0.5f, entity->Position() + 0.5f);
            if (intersects) {
                std::cout << entity->Type() << std::endl;
                std::cout << "contact.normal      : " << contact.normal.x << ' ' << contact.normal.y << std::endl;
                std::cout << "contact.penetration : " << contact.penetration << std::endl;
                if (entity->Type() == "Flame") {
                    Die();
                    continue;
                }
                SetPosition(Position() - contact.normal * (contact.penetration + 0.001f));
                x = 0;
                break;
                //a->NextTransform().SetPosition(glm::vec3(newPos.x, Height(), newPos.y));
                /*auto P = glm::vec2(out.Position().x, out.Position().z);
                auto N = normalize(glm::vec2(out.Normal().x, out.Normal().z));
                auto V = dir;
                auto A = glm::vec2(a->NextTransform().WorldPosition().x, a->NextTransform().WorldPosition().z);
                auto B = ProjectPointOnPlane(A, P, N);
                auto AB = B - A;
                auto ABLen = length(AB);
                auto alpha = glm::acos(dot(V, AB / ABLen));
                auto BCLen = tan(alpha) * ABLen;
                auto ACLen = ABLen * ABLen + BCLen * BCLen;
                auto C = A + V * ACLen;
                auto BC = C - B;
                std::cout << "alpha: " << alpha << '\n';
                std::cout << "dir  : " << dir.x << ' ' << dir.y << '\n';
                std::cout << "N    : " << N.x << ' ' << N.y << '\n';
                std::cout << "P    : " << P.x << ' ' << P.y << '\n';
                std::cout << "A    : " << A.x << ' ' << A.y << '\n';
                std::cout << "B    : " << B.x << ' ' << B.y << '\n';
                std::cout << "C    : " << C.x << ' ' << C.y << '\n';
                if (glm::isnan(alpha) || glm::degrees(alpha) > 45) {
                    foundNewDir = true;
                    dir = BC / BCLen;
                    x = 0;
                    break;
                }*/
                /*if (glm::isnan(alpha) || glm::degrees(alpha) > 45) {
                    std::cout << "BC   : " << BC.x << ' ' << BC.y << '\n';
                    std::cout << "BCLen: " << BCLen << '\n';
                    std::cout << std::endl;
                    dir = BC / BCLen;
                    auto nextPlayerPos = A + (dir * Speed() * delta);
                    a->NextTransform().SetPosition(glm::vec3(nextPlayerPos.x, Height(), nextPlayerPos.y));
                    x = 0;
                    break;
                }  else
                    newPlayerPosition = glm::vec2(a->NextTransform().WorldPosition().x, a->NextTransform().WorldPosition().z);*/
                std::cout << std::endl;
            }
        }
    }
}