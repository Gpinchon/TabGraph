/*
* @Author: gpinchon
* @Date:   2020-08-08 19:59:48
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 22:00:47
*/
#pragma once

#include "Scene/Scene.hpp"

#include <filesystem>

class GameEntity;

class Level : public Scene {
public:
    Level(const std::string& name, const glm::ivec2& size);
    static std::shared_ptr<Level> Parse(const std::filesystem::path path);
    static std::shared_ptr<Level> Create(const std::string& name, const glm::ivec2& size);
    //static std::shared_ptr<Level> CurrentLevel();
    glm::ivec2 Size() const;
    void SetGameEntity(glm::ivec2 position, const std::shared_ptr<GameEntity> entity);
    void SetGameEntityPosition(glm::ivec2 position, std::shared_ptr<GameEntity> entity);
    std::shared_ptr<GameEntity> GetGameEntity(glm::ivec2 position) const;
    void SetSpawnPoint(glm::ivec2);
    glm::ivec2 SpawnPoint() const;
    virtual void Render(const Render::Pass&, const Render::Mode&) override;
    virtual void RenderDepth(const Render::Mode&) override;

private:
    virtual void _UpdateCPU(float delta);
    virtual void _FixedUpdateCPU(float delta);
    const glm::ivec2 _size;
    glm::ivec2 _spawnPoint;
    std::vector<std::shared_ptr<GameEntity>> _entities;
    //static std::shared_ptr<Level> _currentLevel;
};