#include "Camera/Camera.hpp"
#include "Event/Events.hpp"

#include "Game.hpp"
#include "Level.hpp"
#include "Player.hpp"

#include <array>

void GameRefresh(float);

struct GameManager {
    GameManager()
    {
        Events::OnRefresh().Connect(&GameRefresh);
    }
    std::shared_ptr<Level> currentLevel;
    std::array<std::shared_ptr<Player>, 4> players;
    int playerNumber { 0 };
};

void GameRefresh(float delta)
{
    Game::Update(delta);
}

GameManager& getGameManager()
{
    static GameManager gameManager;
    return gameManager;
}

void Game::SetCurrentLevel(std::shared_ptr<Level> currentLevel)
{
    for (auto i = 0; i < PlayerNumber(); ++i) {
        GetPlayer(i)->SetPosition(glm::vec2(currentLevel->SpawnPoint()) + 0.5f);
        //currentLevel->SetGameEntityPosition(, GetPlayer(i));
    }
    getGameManager().currentLevel = currentLevel;
}

std::shared_ptr<Level> Game::CurrentLevel()
{
    return getGameManager().currentLevel;
}

void Game::Update(float step)
{
}

int Game::PlayerNumber()
{
    return getGameManager().playerNumber;
}

std::shared_ptr<Player> Game::GetPlayer(int index)
{
    assert(index < getGameManager().playerNumber);
    return getGameManager().players.at(index);
}

int Game::AddPlayer()
{
    if (getGameManager().playerNumber == 4)
        return -1;
    auto newPlayer = Player::Create(glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f));
    getGameManager().playerNumber++;
    getGameManager().players.at(getGameManager().playerNumber - 1) = newPlayer;
    return getGameManager().playerNumber - 1;
}

void Game::MovePlayer(int index, const glm::vec2& input)
{
}
