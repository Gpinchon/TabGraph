/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:56:25
*/

#pragma once

#include <memory> // for shared_ptr

class Shader; // lines 13-13
class VertexArray; // lines 12-12

/** @brief Render manages the graphical rendering and frame pacing */
namespace Render {
/** @brief Initiates rendering loop */
void Start();
/** @brief Stops rendering loop */
void Stop();
/** @brief Asks for a redraw on next loop */
void RequestRedraw();
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

const std::shared_ptr<VertexArray> DisplayQuad();
};