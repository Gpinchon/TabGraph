/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 15:57:29
*/

#pragma once

#include "Texture.hpp" // for Texture
#include <array> // for array
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Cubemap : public Texture {
public:
    static std::shared_ptr<Cubemap> create(const std::string&);
    static std::shared_ptr<Cubemap> create(const std::string&, std::shared_ptr<Texture> fromTexture);
    static std::shared_ptr<Cubemap> parse(const std::string&, const std::string&);
    static std::shared_ptr<Cubemap> Get(unsigned index);
    static std::shared_ptr<Cubemap> get_by_name(const std::string&);
    void load() override;
    void unload() override;
    std::shared_ptr<Texture> side(unsigned index);
    void set_side(unsigned index, std::shared_ptr<Texture>);

private:
    static std::vector<std::shared_ptr<Cubemap>> _cubemaps;
    std::array<std::weak_ptr<Texture>, 6> _sides;
    Cubemap(const std::string&);
};