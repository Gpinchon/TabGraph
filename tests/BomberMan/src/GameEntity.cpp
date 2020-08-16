/*
* @Author: gpinchon
* @Date:   2020-08-08 20:02:06
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-16 18:17:12
*/

//#include "Physics/RigidBody.hpp"
#include "Transform.hpp"

#include "GameEntity.hpp"

GameEntity::GameEntity(const std::string& name, const std::string& entityType)
    : Node(name)
    , _type(entityType)
{
}

GameEntity::GameEntity(const std::string& entityType)
    : Node("")
    , _type(entityType)
{
}

std::string GameEntity::Type() const
{
    return _type;
}

glm::vec2 GameEntity::Size() const
{
    return _size;
}

float GameEntity::Height() const
{
    return _height;
}

void GameEntity::SetPosition(const glm::vec2& position)
{
    auto pos3D = glm::vec3(position.x, Height() / 2.f, position.y);
    GetComponent<Transform>()->SetPosition(pos3D);
    //GetComponent<RigidBody>()->CurrentTransform().SetPosition(pos3D);
    //GetComponent<RigidBody>()->NextTransform().SetPosition(pos3D);
}

void GameEntity::LookAt(const glm::vec2& position)
{
    auto pos3D = glm::vec3(position.x, Height() / 2.f, position.y);
    GetComponent<Transform>()->LookAt(pos3D);
}

glm::vec2 GameEntity::Position() const
{
    auto position = GetComponent<Transform>()->WorldPosition();
    return glm::vec2(position.x, position.z);
}

void GameEntity::Die()
{
}
