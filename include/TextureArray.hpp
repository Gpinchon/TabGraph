/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:27:10
*/

#pragma once

#include "Texture.hpp"
#include <vector>

class TextureArray : public Texture {
public:
    static std::shared_ptr<TextureArray> create(const std::string& name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
    virtual void set(std::shared_ptr<Texture>, int index);
    //virtual int		add(Texture *texture);
    virtual void load();

protected:
    TextureArray(const std::string& name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
    unsigned _capacity{ 0 };
    std::vector<std::shared_ptr<Texture>> _array;
};
