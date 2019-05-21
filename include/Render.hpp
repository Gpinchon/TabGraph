/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-21 14:26:56
*/

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

class VertexArray;
class Shader;

namespace Render {
	void RequestRedraw();
	void AddPostTreatment(std::shared_ptr<Shader>);
	void RemovePostTreatment(std::shared_ptr<Shader>);
	void Start();
	void Stop();

	void Update();
	void FixedUpdate();
	void scene();
	void add_post_treatment(std::shared_ptr<Shader>);
	void add_post_treatment(const std::string& name, const std::string& path);
	void remove_post_treatment(std::shared_ptr<Shader>);
	void start_rendering_thread();
	void stop_rendering_thread();
	void request_redraw();
	double delta_time();
	bool needs_update();
	uint64_t frame_nbr(void);
	void		SetInternalQuality(float);
	float	InternalQuality();
	const std::shared_ptr<VertexArray> display_quad();
	std::vector<std::weak_ptr<Shader>>& post_treatments();
};