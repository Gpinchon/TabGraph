/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:36
*/

#pragma once

#include "Event/Signal.hpp"
#include <atomic>
#include <memory> // for shared_ptr

class Shader; // lines 13-13
class Geometry; // lines 12-12
class Framebuffer;

/** @brief Render manages the graphical rendering and frame pacing */
namespace Render {
	void Init();
	typedef void* Context;
	Context GetContext();
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

	void Scene();
};