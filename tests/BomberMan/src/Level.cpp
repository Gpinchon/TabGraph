/*
* @Author: gpinchon
* @Date:   2020-08-08 20:11:05
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 22:10:31
*/

#include "Camera/OrbitCamera.hpp"
#include "Light/Light.hpp"
#include "Material.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/PlaneMesh.hpp"
#include "Transform.hpp"

#include "CrispyWall.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"
#include "Wall.hpp"

Level::Level(const std::string& name, const glm::ivec2& size)
    : Scene(name)
    , _size(size)
    , _entities(size.x * size.y)
{
    auto floorMesh = PlaneMesh::Create("FloorMesh", Size());
    auto floorNode = Node::Create("FloorNode");
    floorNode->GetComponent<Transform>()->SetPosition(glm::vec3(Size().x / 2.f, 0, Size().y / 2.f));
    //auto light = DirectionnalLight::Create("MainLight", glm::vec3(1.f), glm::vec3(1.f), 1.f, true);
    auto radius = size.x + size.y;
    auto light = DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(radius), 1, true);
    light->SetLimits(glm::vec4(-radius, radius, -radius, radius));
    auto camera = OrbitCamera::Create("MainCamera", 45.f, glm::pi<float>() / 2.f, 1, radius);
    camera->SetTarget(floorNode);
    //camera->GetComponent<Transform>()->SetPosition(glm::vec3(0, 10, 1));
    //camera->GetComponent<Transform>()->LookAt(glm::vec3(0, 0, 0));
    floorMesh->GetMaterial(0)->SetAlbedo(glm::vec3(1.f));
    floorNode->SetComponent(floorMesh);
    SetCurrentCamera(camera);
    Add(light);
    Add(floorNode);
    /*
    for (auto x = 0; x < Size().x; ++x) {
        if (x != 0 && x != Size().x - 1)
            continue;
        for (auto y = 0; y < Size().y; ++y) {
            if (y != 0 && y != Size().y - 1)
                continue;
            auto wall = Wall::Create();
            wall->GetComponent<Transform>()->SetPosition(glm::vec3(x + 0.5, 0.5, y + 0.5));
            SetGameEntity(glm::ivec2(x, y), wall);
        }
    }*/
    //Add(cameraNode);
}

std::shared_ptr<Level> Level::Create(const std::string& name, const glm::ivec2& size)
{
    return std::shared_ptr<Level>(new Level(name, size));
}

#include <fstream>
#include <sstream>

std::shared_ptr<Level> Level::Parse(const std::string& path)
{
    std::ifstream file;
    file.open(path);
    std::vector<std::vector<int>> map;
    while (file.good()) {
        std::string line;
        std::getline(file, line);
        if (line.find("//") == 0)
            continue;
        std::stringstream iss(line);
        int number;
        std::vector<int> lineNumbers;
        while (iss >> number) {
            lineNumbers.push_back(number);
        }
        map.push_back(lineNumbers);
    }
    int maxLine = 0;
    for (const auto& row : map) {
        if (row.size() > maxLine)
            maxLine = row.size();
    }
    auto level = Create(path, glm::ivec2(map.size(), maxLine));
    for (auto x = 0u; x < map.size(); ++x) {
        for (auto y = 0u; y < map.at(x).size(); ++y) {
            switch (map.at(x).at(y)) {
            case 1: {
                auto wall = Wall::Create();
                level->SetGameEntityPosition(glm::ivec2(x, y), wall);
                break;
            }
            case 2: {
                level->SetSpawnPoint(glm::ivec2(x, y));
                break;
            }
            case 3: {
                auto wall = CrispyWall::Create();
                level->SetGameEntityPosition(glm::ivec2(x, y), wall);
            }
            }
        }
    }
    return level;
}

void Level::SetSpawnPoint(glm::ivec2 spawnPoint)
{
    _spawnPoint = spawnPoint;
}

glm::ivec2 Level::SpawnPoint() const
{
    return _spawnPoint;
}

glm::ivec2 Level::Size() const
{
    return _size;
}

void Level::SetGameEntityPosition(glm::ivec2 position, std::shared_ptr<GameEntity> entity)
{
    entity->SetPosition(glm::vec2(position) + entity->Size() / 2.f);
    SetGameEntity(position, entity);
}

void Level::SetGameEntity(glm::ivec2 position, const std::shared_ptr<GameEntity> entity)
{
    std::cout << position.x << " " << position.y << std::endl;
    position = glm::clamp(position, glm::ivec2(0), Size() - 1);
    _entities.at(position.x + position.y * Size().x) = entity;
}

std::shared_ptr<GameEntity> Level::GetGameEntity(glm::ivec2 position) const
{
    return _entities.at(position.x + position.y * Size().x);
}

void Level::_FixedUpdateCPU(float delta)
{
    //Scene::FixedUpdateCPU(delta);
    for (const auto& entity : _entities) {
        if (entity != nullptr)
            entity->FixedUpdateCPU(delta);
    }
    for (auto index = 0; index < Game::PlayerNumber(); ++index) {
        Game::GetPlayer(index)->FixedUpdateCPU(delta);
    }
    //std::cout << __FUNCTION__ << '\n';
}

void Level::Render(const RenderMod& mod)
{
    Scene::Render(mod);
    for (const auto& entity : _entities) {
        if (entity != nullptr)
            entity->Draw(mod);
    }
    for (auto index = 0; index < Game::PlayerNumber(); ++index) {
        Game::GetPlayer(index)->Draw(mod);
    }
    /*for (auto i = 0; i < Game::PlayerNumber(); ++i) {
        auto player = Game::GetPlayer(i);
        player->Draw(mod);
    }*/
    //std::cout << __FUNCTION__ << '\n';
}

void Level::RenderDepth(const RenderMod& mod)
{
    Scene::RenderDepth(mod);
    for (const auto& entity : _entities) {
        if (entity != nullptr)
            entity->DrawDepth(mod);
    }
    for (auto index = 0; index < Game::PlayerNumber(); ++index) {
        Game::GetPlayer(index)->DrawDepth(mod);
    }
    /*for (auto i = 0; i < Game::PlayerNumber(); ++i) {
        auto player = Game::GetPlayer(i);
        player->DrawDepth(mod);
    }*/
    //std::cout << __FUNCTION__ << '\n';
}