/*
* @Author: gpinchon
* @Date:   2021-05-24 13:38:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 13:44:19
*/

#pragma once

#include <Engine.hpp>
#include <Event/InputDevice/InputDevice.hpp>

struct Engine::Impl : private InputDevice {
    Impl(std::shared_ptr<Renderer::FrameRenderer> frameRenderer);
    void Start();
    inline auto Stop() {
        _loop = false;
    }
    inline auto SetCurrentScene(std::shared_ptr<Scene> scene) {
        _currentScene = scene;
    }
    inline auto GetCurrentScene() const {
        return _currentScene;
    }
    inline auto SetFrameRenderer(std::shared_ptr<Renderer::FrameRenderer> frameRenderer) {
        _frameRenderer = frameRenderer;
    }
    inline auto GetFrameRenderer() const {
        return _frameRenderer;
    }
    inline auto& OnFixedUpdate() {
        return _onFixedUpdate;
    }
    inline auto& OnUpdate() {
        return _onUpdate;
    }
    static std::filesystem::path GetProgramPath();
    static std::filesystem::path GetExecutionPath();

private:
    std::atomic<bool> _loop{ true };
    Signal<float> _onFixedUpdate;
    Signal<float> _onUpdate;
    std::shared_ptr<Renderer::FrameRenderer> _frameRenderer;
    std::shared_ptr<Scene> _currentScene;

    // Hérité via InputDevice
    virtual void ProcessEvent(const Event&) override;
};