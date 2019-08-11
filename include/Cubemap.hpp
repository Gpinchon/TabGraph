/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:15:11
*/

#pragma once

#include "Texture.hpp" // for Texture
#include <array> // for array
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Cubemap : public Texture {
public:
    static std::shared_ptr<Cubemap> Create(const std::string&);
    static std::shared_ptr<Cubemap> Create(const std::string&, std::shared_ptr<Texture> fromTexture);
    static std::shared_ptr<Cubemap> parse(const std::string&, const std::string&);
    static std::shared_ptr<Cubemap> Get(unsigned index);
    static std::shared_ptr<Cubemap> GetByName(const std::string&);
    static void Add(std::shared_ptr<Cubemap>);
    void load() override;
    void unload() override;
    std::shared_ptr<Texture> side(unsigned index);
    void set_side(unsigned index, std::shared_ptr<Texture>);

private:
    static std::vector<std::shared_ptr<Cubemap>> _cubemaps;
    std::array<std::weak_ptr<Texture>, 6> _sides;
    Cubemap(const std::string&);
};