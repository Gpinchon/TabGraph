/*
* @Author: gpinchon
* @Date:   2020-08-08 19:50:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-13 23:42:47
*/

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "Engine.hpp"
#include "Config.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include <iostream>

int main(int /*argc*/, char const** /*argv*/)
{
    std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
    Config::Parse(Engine::ResourcePath() / "config.ini");
    Engine::Init();
    auto level = Level::Parse(Engine::ResourcePath() / "maps/test.map");
    Game::AddPlayer();
    Game::SetCurrentLevel(level);
    Scene::SetCurrent(level);
    Engine::Start();
    return 0;
}