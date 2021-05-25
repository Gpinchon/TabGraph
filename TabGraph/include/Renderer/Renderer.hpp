/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 19:02:51
*/

#pragma once

#include <Event/Signal.hpp>

#include <glm/fwd.hpp>
#include <atomic>
#include <memory> // for shared_ptr

namespace Shader {
class Program;
};
class Geometry; // lines 12-12
class Framebuffer;
class Texture2D;
class Camera;
class Scene;
struct Window;

/** @brief Renderer manages the graphical rendering and frame pacing */
namespace Renderer {
/**
 * @brief see https://www.khronos.org/opengl/wiki/Swap_Interval
*/
enum class SwapInterval {
    Adaptative = -1, //Adaptative V-Sync
    Immediate, //No V-Sync
    Synchronized //Enable V-Sync
};
struct FrameRenderer;
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
struct FrameRenderer : std::enable_shared_from_this<FrameRenderer> {
    static std::shared_ptr<FrameRenderer> Create(std::weak_ptr<Window> window);
    void SetViewPort(const glm::ivec2& min, const glm::ivec2 &max);
    void SetViewPort(const glm::ivec2& size);
    uint32_t GetFrameNumber() const;
    const std::shared_ptr<Window> GetWindow() const;
    const std::shared_ptr<Geometry> GetDisplayQuad() const;
    const std::shared_ptr<Texture2D> GetDefaultBRDFLUT() const;
    void RenderFrame(std::shared_ptr<Scene> scene);
    /**
     * @brief Sets the swap interval of this FrameRenderer
     * @param swapInterval : see https://www.khronos.org/opengl/wiki/Swap_Interval
    */
    void SetSwapInterval(SwapInterval swapInterval);
    /**
     * @return the current swap interval of this FrameBuffer
    */
    SwapInterval GetSwapInterval() const;
    //----------------Deferred Path-----------------------------
    /**
     * Deferred Render Target :
     * RT0 : BRDF CDiff (RGB), Ambient Occlusion (A) //Uint8_NormalizedRGBA
     * RT1 : BRDF F0 (RGB), BRDF Alpha (A)           //Uint8_NormalizedRGBA
     * RT2 : Normal (RGB)                            //Int16_NormalizedRGB
     * RT3 : Velocity (RG)                           //Float16_RG
     * RT4 : Color (Unlit/Emissive/Final Color)      //Float16_RGB
    */
    std::shared_ptr<Framebuffer> DeferredGeometryBuffer();
    /**
     * Deferred Lighting Render Target
     * RT0 : Diffuse    //Float16_RGB
     * RT1 : Reflection //Float16_RGB
    */
    std::shared_ptr<Framebuffer> DeferredLightingBuffer();
    //----------------------------------------------------------

    //----------------Forward Path------------------------------
    /**
     * Forward Transparent Render Target :
     * RT0 : Weighted Color (RGB), Opacity (A)        //Float16_RGBA
     * RT1 : Alpha Coverage                           //Uint8_NormalizedR
     * RT2 : Distortion                               //Uint8_NormalizedRGB
     * RT3 : Transmission Color (RGB), BRDF Alpha (A) //Uint8_NormalizedRGBA
    */
    std::shared_ptr<Framebuffer> ForwardTransparentRenderBuffer();
    //----------------------------------------------------------

    /**
     * Opaque Render Target :
     * RT0 : Color (Unlit/Emissive/Final Color) //Float16_RGB
    */
    std::shared_ptr<Framebuffer> OpaqueRenderBuffer();
    /**
     * RT0 : Final Render Buffer //Uint8_NormalizedRGB
    */
    std::shared_ptr<Framebuffer> FinalRenderBuffer();

    /**
     * RT0 : Previous Frame Render Buffer //Uint8_NormalizedRGB
    */
    std::shared_ptr<Framebuffer> PreviousRenderBuffer();

private:
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;
    FrameRenderer(std::weak_ptr<Window> window);
};
};