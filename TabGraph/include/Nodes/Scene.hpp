/*
* @Author: gpinchon
* @Date:   2021-06-19 15:05:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:44
*/

#include <Nodes/Renderable.hpp>
#include <Core/Inherit.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Surfaces {
    class Skybox;
};
namespace Nodes {
    class Camera;
};
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Scene : public Core::Inherit<Renderable, Scene> {
public:
    Scene();
    Scene(const std::string& name)
        : Scene()
    {
        SetName(name);
    }
    inline void Add(std::shared_ptr<Node> node)
    {
        node->SetParent(shared_from_this());
    }
    inline void Remove(std::shared_ptr<Node> node)
    {
        if (node->GetParent().get() == this)
            node->SetParent(nullptr);
    }
    /**
	 * @return the current camera being used by this Scene
	*/
    inline auto GetCamera() const
    {
        return _camera;
    }
    /**
	 * @param camera : the camera the Scene will be seen from
	*/
    inline void SetCamera(std::shared_ptr<Nodes::Camera> camera)
    {
        _camera = camera;
    }
    inline void SetSkybox(std::shared_ptr<Surfaces::Skybox> skybox)
    {
        _skybox = skybox;
    }

private:
    std::shared_ptr<Nodes::Camera> _camera;
    std::shared_ptr<Surfaces::Skybox> _skybox;
};
};
