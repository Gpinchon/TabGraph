/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-22 15:48:05
*/

#pragma once
#include <memory>
#include <vector>

class VertexArray;
class Shader;

/** @brief Render manages the graphical rendering and frame pacing */
namespace Render {
	/** @brief Initiates rendering loop */
	void 	Start();
	/** @brief Stops rendering loop */
	void 	Stop();
	/** @brief Asks for a redraw on next loop */
	void 	RequestRedraw();
	/**
	*	@brief Adds a post-treatment Shader to be applied after rasterization path
	*	@param shader The post-treatment Shader to add
	*/
	void 	AddPostTreatment(std::shared_ptr<Shader> shader);
	/**
	*	@brief Removes a post-treatment Shader from the list
	*	@param shader The post-treatment Shader to remove
	*/
	void 	RemovePostTreatment(std::shared_ptr<Shader> shader);
	/**
	*	@brief Set the rendering scale factor
	*	@param quality The new rendering scale factor
	*/
	void	SetInternalQuality(float quality);
	/**
	*	@brief Returns the rendering scale factor
	*	@return The current rendering scale factor
	*/
	float	InternalQuality();
	const std::shared_ptr<VertexArray> DisplayQuad();
};