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

float Halton(int b, int i)
{
    float r = 0.0;
    float f = 1.0;
    while (i > 0) {
        f = f / float(b);
        r = r + f * float(i % b);
        i = int(floor(float(i) / float(b)));
    }
    return r;
}

float Halton2(int i)
{
    return Halton(2, i);
}

glm::vec2 Halton23(int i)
{
    return glm::vec2(Halton2(i), Halton(3, i));
}

int main(int /*argc*/, char const** /*argv*/)
{
    /*std::cout << "const vec2 haltonSequence[256] = vec2[256](\n";
    for (int i = 1; i <= 256; ++i) {
        auto halton = Halton23(i) * 2.f - 1.f;
        std::cout << "vec2(" << halton.x << ", " << halton.y << "),\n";
    }
    std::cout << ");" << std::endl;
    return 0;*/
    std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
    Config::Global().Parse(Engine::ResourcePath() / "config.ini");
    Engine::Init();
    auto level = Level::Parse(Engine::ResourcePath() / "maps/test.map");
    Game::AddPlayer();
    Game::SetCurrentLevel(level);
    Scene::SetCurrent(level);
    Engine::Start();
    return 0;
}