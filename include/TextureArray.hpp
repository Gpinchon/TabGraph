/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:42:41
*/

#pragma once

#include <GL/glew.h>    // for GLenum
#include <memory>       // for shared_ptr
#include <string>       // for string
#include <vector>       // for vector
#include "Texture.hpp"  // for Texture
#include "vml.h"        // for VEC2

class TextureArray : public Texture {
public:
    static std::shared_ptr<TextureArray> create(const std::string& name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
    virtual void set(std::shared_ptr<Texture>, int index);
    //virtual int		add(Texture *texture);
    virtual void load() override;

protected:
    TextureArray(const std::string& name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
    unsigned _capacity{ 0 };
    std::vector<std::shared_ptr<Texture>> _array;
};
