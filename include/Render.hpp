/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-22 15:41:29
*/

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

class VertexArray;
class Shader;

/** @brief Render manages the graphical rendering and frame pacing
*/
namespace Render {
	/** @brief Initiates rendering loop */
	void 	Start();
	/** @brief Stops rendering loop */
	void 	Stop();
	/** @brief Asks for a redraw on next loop */
	void 	RequestRedraw();
	/**
	*	@brief Adds a post treatment to be applied after rasterization path
	*	@param s The post-treatment shader to add
	*/
	void 	AddPostTreatment(std::shared_ptr<Shader> s);
	/**
	*	@brief Removes a post treatment from the list
	*	@param s The post-treatment shader to remove
	*/
	void 	RemovePostTreatment(std::shared_ptr<Shader> s);
	/**
	*	@brief Set the rendering scale factor
	*	@param q The new rendering scale factor
	*/
	void	SetInternalQuality(float q);
	/**
	*	@brief Returns the rendering scale factor
	*	@return The current rendering scale factor
	*/
	float	InternalQuality();
	const std::shared_ptr<VertexArray> DisplayQuad();
};