#pragma once

#include "Scene/Scene.hpp"

class GameEntity;

class Level : public Scene {
public:
    static std::shared_ptr<Level> Parse(const std::string& path);
    static std::shared_ptr<Level> Create(const std::string& name, const glm::ivec2& size);
    //static std::shared_ptr<Level> CurrentLevel();
    glm::ivec2 Size() const;
    void SetGameEntity(glm::ivec2 position, const std::shared_ptr<GameEntity> entity);
    void SetGameEntityPosition(glm::ivec2 position, std::shared_ptr<GameEntity> entity);
    std::shared_ptr<GameEntity> GetGameEntity(glm::ivec2 position) const;
    void SetSpawnPoint(glm::ivec2);
    glm::ivec2 SpawnPoint() const;
    virtual void FixedUpdateCPU(float delta) override;
    virtual void Render(const RenderMod&) override;
    virtual void RenderDepth(const RenderMod&) override;

private:
    Level(const std::string& name, const glm::ivec2& size);
    const glm::ivec2 _size;
    glm::ivec2 _spawnPoint;
    std::vector<std::shared_ptr<GameEntity>> _entities;
    //static std::shared_ptr<Level> _currentLevel;
};