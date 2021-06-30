/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 19:02:51
*/

#pragma once

#include <glm/mat4x4.hpp>
#include <memory>

class Surface;
class Camera;
class Scene;

/** @brief Renderer manages the graphical rendering and frame pacing */
namespace TabGraph::Renderer {
class FrameRenderer;
struct Options {
    enum class Pass {
        BeforeRender,
        DeferredGeometry,
        DeferredLighting,
        DeferredMaterial,
        ForwardTransparent,
        ForwardOpaque,
        ShadowDepth,
        AfterRender,
        MaxValue
    };
    enum class Mode {
        All,
        Opaque,
        Transparent,
        None,
        MaxValue
    };
    Options(Options::Pass pass, Options::Mode mode,
        std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene, std::shared_ptr<FrameRenderer> renderer,
        uint32_t frameNbr, float delta)
        : pass(pass)
        , mode(mode)
        , camera(camera)
        , scene(scene)
        , renderer(renderer)
        , frameNumber(frameNbr)
        , delta(delta) {};
    Pass pass;
    Mode mode;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<FrameRenderer> renderer;
    uint32_t frameNumber;
    float delta;
};
struct ShapeState {
    glm::mat4 transform;
    glm::mat4 prevTransform;
    std::weak_ptr<Surface> surface;
};
};