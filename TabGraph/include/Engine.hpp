/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 13:43:29
*/

#pragma once

#include <Event/Signal.hpp>

#include <filesystem>
#include <string> // for string
#include <sys/types.h> // for int8_t

namespace Renderer {
struct FrameRenderer;
}

class Scene;

/** @brief This class orchestrates the rendering loop and logics around it
*/
class Engine {
public:
    static std::shared_ptr<Engine> Create(std::shared_ptr<Renderer::FrameRenderer> frameRenderer);
    void SetCurrentScene(std::shared_ptr<Scene>);
    std::shared_ptr<Scene> GetCurrentScene() const;
    void SetFrameRenderer(std::shared_ptr<Renderer::FrameRenderer>);
    std::shared_ptr<Renderer::FrameRenderer> GetFrameRenderer() const;
    /** @brief Starts Engine loop */
    void Start(void);
    /** @brief Tells the Engine to stop the loop */
    void Stop(void);
    Signal<float>& OnFixedUpdate();
    Signal<float>& OnUpdate();

    /** @return See https://wiki.libsdl.org/SDL_GetBasePath */
    static const std::filesystem::path GetProgramPath();
    /** @return See http://manpagesfr.free.fr/man/man3/getcwd.3.html */
    static const std::filesystem::path GetExecutionPath();
    /** @return Program path + "/res/" */
    static const std::filesystem::path GetResourcePath(void);

private:
    Engine(std::shared_ptr<Renderer::FrameRenderer> frameRenderer);
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;
};
