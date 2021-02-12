/*
* @Author: gpinchon
* @Date:   2020-08-08 20:11:05
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 22:10:31
*/

#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Camera/OrbitCamera.hpp"
#include "Engine.hpp"
#include "Environment.hpp"
#include "Light/DirectionnalLight.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/PlaneMesh.hpp"
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp"

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
    //Engine::OnFixedUpdate().ConnectMember(this, &Level::_FixedUpdateCPU);
    //Engine::OnUpdate().ConnectMember(this, &Level::_UpdateCPU);
}

#include "Texture/Texture2D.hpp"

std::shared_ptr<Level> Level::Create(const std::string& name, const glm::ivec2& size)
{
    auto level(Component::Create<Level>(name, size));
    auto floorMesh = PlaneMesh::Create("FloorMesh", level->Size());
    auto floorNode = Component::Create<Node>("FloorNode");
    floorNode->SetPosition(glm::vec3(level->Size().x / 2.f, 0, level->Size().y / 2.f));
    //auto light = DirectionnalLight::Create("MainLight", glm::vec3(1.f), glm::vec3(1.f), 1.f, true);
    auto radius = size.x + size.y;
    auto light = Component::Create<DirectionnalLight>("MainLight", glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), true);
    light->SetHalfSize(glm::vec3(size.x / 2.f, 1.5, size.y / 2.f));
    light->SetPosition(glm::vec3(size.x / 2.f, 0, size.y / 2.f));
    light->SetInfinite(false);
    auto camera = Component::Create<OrbitCamera>("MainCamera", 45.f, glm::pi<float>() / 2.f, 1, radius / 2.f);
    camera->SetTarget(floorNode);
    
    //auto floorTexture = Component::Create<Texture2D>("floorTexture", size, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE, data.data());
    //floorTexture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //floorTexture->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //camera->GetComponent<Transform>()->SetPosition(glm::vec3(0, 10, 1));
    //camera->GetComponent<Transform>()->LookAt(glm::vec3(0, 0, 0));
    floorMesh->GetMaterial(0)->SetDiffuse(glm::vec3(1.f));
    //floorMesh->GetMaterial(0)->SetTextureDiffuse(floorTexture);
    floorNode->SetComponent(floorMesh);
    level->SetCurrentCamera(camera);
    level->Add(light);
    level->Add(floorNode);
    return level;
}

#include <fstream>
#include <sstream>

std::shared_ptr<Level> Level::Parse(const std::filesystem::path path)
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
    auto size = glm::ivec2(map.size(), maxLine);
    auto level = Create(path.string(), size);
    //auto textureData = std::vector<glm::u8vec4>(size_t(size.x) * size.y);
    auto floorImage = Component::Create<Image>(size, Pixel::SizedFormat::Uint8_NormalizedRGBA);
    for (auto y = 0; y < size.y; ++y) {
        for (auto x = 0; x < size.x; ++x) {
            auto index = x + y * size.x;
            auto color1 = glm::vec4(0.133333, 0.270588, 0.000000, 1);
            auto color0 = glm::vec4(0.192156, 0.388235, 0.000000, 1);
            auto color{ index % 2 ? color1 : color0 };
            floorImage->SetColor(glm::ivec2(x, y), color);
        }
    }
    for (auto x = 0u; x < map.size(); ++x) {
        for (auto y = 0u; y < map.at(x).size(); ++y) {
            auto index = x + y * size.x;
            switch (map.at(x).at(y)) {
            case 1: {
                auto wall = Wall::Create();
                level->SetGameEntityPosition(glm::ivec2(x, y), wall);
                auto color{ glm::vec4(0.274509, 0.050980, 0.050980, 1) };
                floorImage->SetColor(glm::ivec2(x, y), color);
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
    auto floorImageAsset{ Component::Create<Asset>() };
    floorImageAsset->AddComponent(floorImage);
    floorImageAsset->SetLoaded(true);
    auto floorTexture = Component::Create<Texture2D>(floorImageAsset);
    floorTexture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    floorTexture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    level->GetComponentInChildrenByName<Mesh>("FloorMesh")->GetMaterial(0)->SetTextureDiffuse(floorTexture);
    struct dirent* e;
    std::filesystem::path folder;

    folder = path.parent_path() / "env/hdr/";
    std::cout << folder << std::endl;
    auto newEnv = Component::Create<Environment>("Environment");
    newEnv->SetDiffuse(Component::Create<Cubemap>(Component::Create<Asset>(folder / "diffuse.hdr")));
    newEnv->SetIrradiance(Component::Create<Cubemap>(Component::Create<Asset>(folder / "environment.hdr")));
    level->SetEnvironment(newEnv);
    
    level->SetEnvironment(level->GetComponent<Environment>(0));
    std::cout << level->GetEnvironment() << std::endl;
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
    position = glm::clamp(position, glm::ivec2(0), Size() - 1);
    _entities.at(position.x + position.y * Size().x) = entity;
}

std::shared_ptr<GameEntity> Level::GetGameEntity(glm::ivec2 position) const
{
    return _entities.at(position.x + position.y * Size().x);
}

void Level::_UpdateCPU(float delta)
{
    //for (const auto& entity : _entities) {
    //    if (entity != nullptr)
    //        entity->UpdateCPU(delta);
    //}
    //for (auto index = 0; index < Game::PlayerNumber(); ++index) {
    //    Game::GetPlayer(index)->UpdateCPU(delta);
    //}
}

void Level::_FixedUpdateCPU(float delta)
{
    //Scene::FixedUpdateCPU(delta);
    //for (const auto& entity : _entities) {
    //    if (entity != nullptr)
    //        entity->FixedUpdateCPU(delta);
    //}
    //for (auto index = 0; index < Game::PlayerNumber(); ++index) {
    //    Game::GetPlayer(index)->FixedUpdateCPU(delta);
    //}
}

void Level::Render(const RenderPass& pass, const RenderMod& mod)
{
    Scene::Render(pass, mod);
    for (const auto& entity : _entities) {
        if (entity != nullptr)
            entity->Draw(std::static_pointer_cast<Scene>(shared_from_this()), pass, mod, true);
    }
    for (auto index = 0; index < Game::PlayerNumber(); ++index) {
        auto player(Game::GetPlayer(index));
        player->Draw(std::static_pointer_cast<Scene>(shared_from_this()), pass, mod, true);
    }
}

void Level::RenderDepth(const RenderMod& mod)
{
    Scene::RenderDepth(mod);
    for (const auto& entity : _entities) {
        if (entity != nullptr)
            entity->DrawDepth(std::static_pointer_cast<Scene>(shared_from_this()), mod, true);
    }
    for (auto index = 0; index < Game::PlayerNumber(); ++index) {
        Game::GetPlayer(index)->DrawDepth(std::static_pointer_cast<Scene>(shared_from_this()), mod, true);
    }
}