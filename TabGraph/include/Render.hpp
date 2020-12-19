/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:56:25
*/

#pragma once

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
/**
	*	@brief Set the rendering scale factor
	*	@param quality The new rendering scale factor
	*/
void SetInternalQuality(float quality);
/**
	*	@brief Returns the rendering scale factor
	*	@return The current rendering scale factor
	*/
float InternalQuality();
/**
	*	@brief Returns the delta time between the last two frames
	*	@return The delta time between the two ast frames
	*/
double DeltaTime();

/**
	*	@brief Returns the delta time between the last two fixed updates
	*	@return The delta time between the last two fixed updates (usually 0.015)
	*/
double FixedDeltaTime();

uint32_t FrameNumber();


const std::shared_ptr<Framebuffer> OpaqueBuffer();
const std::shared_ptr<Framebuffer> GeometryBuffer();
const std::shared_ptr<Framebuffer> LightBuffer();

const std::shared_ptr<Geometry> DisplayQuad();
};