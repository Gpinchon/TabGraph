/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:56:25
*/

#pragma once

#include "Event/Signal.hpp"
#include <memory> // for shared_ptr
#include <atomic>

class Shader; // lines 13-13
class Geometry; // lines 12-12
class Framebuffer;

/** @brief Render manages the graphical rendering and frame pacing */
namespace Render {
/** @brief Initiates rendering loop */
void Start();
/** @brief Stops rendering loop */
void Stop();
/** @brief Asks for a redraw on next loop */
void RequestRedraw();
std::atomic<bool>& NeedsUpdate();
/** @brief Indicates if the rendering still needs update */
std::atomic<bool>& Drawing();
/**
	*	@brief Adds a post-treatment Shader to be applied after rasterization path
	*	@param shader The post-treatment Shader to add
	*/
void AddPostTreatment(std::shared_ptr<Shader> shader);
/**
	*	@brief Removes a post-treatment Shader from the list
	*	@param shader The post-treatment Shader to remove
	*/
void RemovePostTreatment(std::shared_ptr<Shader> shader);

uint32_t FrameNumber();


const std::shared_ptr<Framebuffer> OpaqueBuffer();
const std::shared_ptr<Framebuffer> GeometryBuffer();
const std::shared_ptr<Framebuffer> LightBuffer();

const std::shared_ptr<Geometry> DisplayQuad();

Signal<float>& OnBeforeRender();
Signal<float>& OnAfterRender();
};