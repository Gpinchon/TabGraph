/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:02
*/

#pragma once

#include "Texture/Texture.hpp" // for Texture
#include <GL/glew.h> // for GLenum
#include <glm/glm.hpp> // for glm::vec2
#include <memory> // for shared_ptr
#include <string> // for string
#include <vector> // for vector

class TextureArray : public Texture {
public:
    TextureArray(const std::string& name, glm::ivec2 s, GLenum target, GLenum fi, unsigned capacity);
    virtual void set(std::shared_ptr<Texture>, int index);
    //virtual int		AddTexture *texture);
    virtual void load() override;

protected:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<TextureArray>(*this);
    }
    unsigned _capacity { 0 };
    std::vector<std::shared_ptr<Texture>> _array;
};
