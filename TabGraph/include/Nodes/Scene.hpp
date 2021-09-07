/*
* @Author: gpinchon
* @Date:   2021-06-19 15:05:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:44
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Nodes/Group.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Animations {
class Animation;
}
namespace Lights {
class Light;
}
namespace Shapes {
class Skybox;
}
namespace Cameras {
class Camera;
}
namespace Renderer {
class SceneRenderer;
}
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Scene : public Core::Inherit<Group, Scene> {
    /** @bruef the camera the Scene will be seen from */
    PROPERTY(std::shared_ptr<Cameras::Camera>, Camera, nullptr);
    PROPERTY(std::shared_ptr<Shapes::Skybox>, Skybox, nullptr);
public:
    Scene();
    Scene(const std::string& name)
        : Scene()
    {
        SetName(name);
    }
    ~Scene();
    auto& GetRenderer() const {
        return _renderer;
    }
    inline void Add(std::shared_ptr<Node> node)
    {
        node->SetParent(std::static_pointer_cast<Group>(shared_from_this()));
    }
    inline void Remove(std::shared_ptr<Node> node)
    {
        if (node->GetParent().get() == this)
            node->SetParent(nullptr);
    }
    inline auto GetAnimations() const {
        return _animations;
    }
    inline auto GetLights() const {
        return _lights;
    }
    inline auto Add(std::shared_ptr<Animations::Animation> animation) {
        _animations.push_back(animation);
    }
    inline auto Add(std::shared_ptr<Lights::Light> light) {
        _lights.push_back(light);
    }

private:
    Renderer::SceneRenderer& _renderer;
    std::vector<std::shared_ptr<Animations::Animation>> _animations;
    std::vector<std::shared_ptr<Lights::Light>> _lights;
};
};
