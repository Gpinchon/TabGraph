/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 18:19:49
*/

#pragma once

#include <memory>    // for shared_ptr, weak_ptr
#include <string>    // for string
#include <vector>    // for vector
#include "Node.hpp"  // for Node
#include "glm/glm.hpp"     // for glm::vec3

class Framebuffer;

enum LightType {
    Point,
    Directionnal
};

class Light : public Node {
public:
    static std::shared_ptr<Light> create(const std::string& name, glm::vec3 color, glm::vec3 position, float power);
    static std::shared_ptr<Light> get_by_name(const std::string&);
    static std::shared_ptr<Light> Get(unsigned index);
    static void add(std::shared_ptr<Light>);
    virtual void render_shadow();
    glm::vec3& color();
    float& power();
    bool& cast_shadow();
    std::shared_ptr<Framebuffer> render_buffer();
    //static TextureArray				*shadow_array();
    virtual LightType type();

protected:
    Light(const std::string& name);
    glm::vec3 _color{ 0, 0, 0 };
    float _power{ 0 };
    bool _cast_shadow{ false };
    std::weak_ptr<Framebuffer> _render_buffer;

private:
    static std::vector<std::shared_ptr<Light>> _lights;
};

class DirectionnalLight : public Light {
public:
    static std::shared_ptr<DirectionnalLight> create(const std::string& name, glm::vec3 color, glm::vec3 position, float power, bool cast_shadow = false);
    virtual void render_shadow() override;
    virtual void transform_update() override;
    virtual LightType type() override;

protected:
    DirectionnalLight(const std::string& name);
};
