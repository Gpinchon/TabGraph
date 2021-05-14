/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:26
*/

#pragma once

#include "Surface/Surface.hpp" // for Object
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class TextureCubemap;
namespace Shader {
class Program;
}

namespace Renderer {
class SkyboxRenderer;
}

class Skybox : public Surface {
public:
    PROPERTY(std::shared_ptr<TextureCubemap>, Texture, nullptr);

public:
    Skybox(const std::string& name);
    Skybox(const std::string& name, std::shared_ptr<TextureCubemap> color);
    Skybox(const Skybox& other);
    void Load();
    void Unload();

protected:
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto env(Component::Create<Skybox>(*this));
        return env;
    }
};
