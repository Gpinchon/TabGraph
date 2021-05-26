#include <Camera/Camera.hpp>
#include <Engine.hpp>
#include <Renderer/Renderer.hpp>
#include <Window.hpp>

#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <array>

struct GameManager {
    GameManager()
        : engine(Engine::Create(Renderer::FrameRenderer::Create(Window::Create("Bomberman", glm::ivec2(1280, 720)))))
    {
        engine->OnFixedUpdate().Connect(&Game::Update);
    }
    std::shared_ptr<Level> currentLevel;
    std::shared_ptr<Engine> engine;
    int playerNumber { 0 };
};

static std::unique_ptr<GameManager> s_gameManager;

void GameRefresh(float delta)
{
    Game::Update(delta);
}

GameManager& getGameManager()
{
    return *s_gameManager;
}

void Game::SetCurrentLevel(std::shared_ptr<Level> currentLevel)
{
    getGameManager().currentLevel = currentLevel;
    getGameManager().engine->SetCurrentScene(currentLevel);
}

std::shared_ptr<Level> Game::CurrentLevel()
{
    return getGameManager().currentLevel;
}

void Game::Update(float step)
{
    getGameManager().currentLevel->Update(step);
}

int Game::PlayerNumber()
{
    return getGameManager().playerNumber;
}

int Game::AddPlayer()
{
    if (getGameManager().playerNumber == 4)
        return -1;
    getGameManager().playerNumber++;
    return getGameManager().playerNumber - 1;
}

void Game::MovePlayer(int index, const glm::vec2& input)
{
}

void Game::Init()
{
    if (s_gameManager == nullptr)
        s_gameManager.reset(new GameManager);
}

void Game::Start()
{
    s_gameManager->engine->Start();
}
