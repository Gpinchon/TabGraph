#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <set>
#include <string>
#include <memory>

class Camera;
class Light;
class Node;
class Animation;
enum class RenderMod;

/**
 * @brief scene container
 */
struct Scene
{
	static std::shared_ptr<Scene> Create(const std::string &name);
	static std::shared_ptr<Scene> Current();
	static void SetCurrent(std::shared_ptr<Scene>);
	std::shared_ptr<Camera> CurrentCamera() const;
	void SetCurrentCamera(std::shared_ptr<Camera>);
	~Scene() = default;
	std::string Name() const;
	void SetName(const std::string &name);
	void Add(std::shared_ptr<Node>);
	void Add(std::shared_ptr<Animation>);
	void Update();
	void FixedUpdate();
	void Render(const RenderMod &);
	void RenderDepth(const RenderMod &);
	std::shared_ptr<Node> GetNode(std::shared_ptr<Node>) const;
	std::shared_ptr<Node> GetNodeByName(const std::string &) const;
	std::shared_ptr<Light> GetLightByName(const std::string &) const;
	std::shared_ptr<Camera> GetCameraByName(const std::string &) const;
	const std::vector<std::shared_ptr<Node>> &Nodes();
	const std::vector<std::shared_ptr<Light>> &Lights();
	const std::vector<std::shared_ptr<Camera>> &Cameras();
	const std::vector<std::shared_ptr<Animation>> &Animations();
	glm::vec3 Up() const;
	void SetUp(glm::vec3);

private:
	Scene(const std::string &name);
	glm::vec3 _up {0, 1, 0};
	std::string _name;
	std::vector<std::shared_ptr<Animation>> _animations;
	std::vector<std::shared_ptr<Node>> _nodes;
	std::vector<std::shared_ptr<Light>> _lights;
	std::vector<std::shared_ptr<Camera>> _cameras;
	std::shared_ptr<Camera> _currentCamera;
};