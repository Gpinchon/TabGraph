#pragma once

#include <set>
#include <string>
#include <memory>

class Camera;
class Light;
class Node;
enum class RenderMod;

/**
 * @brief scene container
 */
struct Scene
{
	static std::shared_ptr<Scene> Create(const std::string &name);
	static std::shared_ptr<Scene> Current();
	static void SetCurrent(std::shared_ptr<Scene>);
	std::shared_ptr<Camera> CurrentCamera();
	void SetCurrentCamera(std::shared_ptr<Camera>);
	~Scene() = default;
	std::string Name() const;
	void SetName(const std::string &name);
	void Add(std::shared_ptr<Node>);
	void Update();
	void FixedUpdate();
	void Render(const RenderMod &);
	void RenderDepth(const RenderMod &);
	std::shared_ptr<Node> GetNodeByName(const std::string &);
	std::shared_ptr<Light> GetLightByName(const std::string &);
	std::shared_ptr<Camera> GetCameraByName(const std::string &);
	const std::set<std::shared_ptr<Node>> &Nodes();
	const std::set<std::shared_ptr<Light>> &Lights();
	const std::set<std::shared_ptr<Camera>> &Cameras();
private:
	Scene(const std::string &name);
	std::string _name;
	std::set<std::shared_ptr<Node>> _nodes;
	std::set<std::shared_ptr<Light>> _lights;
	std::set<std::shared_ptr<Camera>> _cameras;
	std::shared_ptr<Camera> _currentCamera;
};