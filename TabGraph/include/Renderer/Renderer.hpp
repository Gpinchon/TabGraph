/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-11 00:15:57
*/

#pragma once

#include "Event/Signal.hpp"
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

/** @brief Renderer manages the graphical rendering and frame pacing */
namespace Renderer {
struct Options {
    enum class Pass {
        DeferredGeometry,
        DeferredLighting,
        DeferredMaterial,
        ForwardTransparent,
        ForwardOpaque,
        ShadowDepth,
        MaxValue
    };
    enum class Mode {
        All,
        Opaque,
        Transparent,
        MaxValue
    };
    Options(Options::Pass pass, Options::Mode mode, std::shared_ptr<Camera> camera, std::shared_ptr<Scene> scene, uint32_t frameNbr)
        : pass(pass)
        , mode(mode)
        , camera(camera)
        , scene(scene)
        , frameNumber(frameNbr) {};
    Options::Pass pass;
    Options::Mode mode;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;
    uint32_t frameNumber;
};
class Context;

uint32_t FrameNumber();

void Init();
Context& GetContext();

const std::shared_ptr<Geometry> DisplayQuad();

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

std::shared_ptr<Texture2D> DefaultBRDFLUT();

void RenderFrame();
};