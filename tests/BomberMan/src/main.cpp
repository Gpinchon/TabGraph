/*
* @Author: gpinchon
* @Date:   2020-08-08 19:50:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-13 23:42:47
*/

#include "Engine.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include <iostream>

int main(int /*argc*/, char const** /*argv*/)
{
    std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
    Engine::Init();
    auto level = Level::Parse("./res/maps/test.map");
    Game::AddPlayer();
    Game::SetCurrentLevel(level);
    Scene::SetCurrent(level);
    Engine::Start();
    return 0;
}