/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:36
*/

#pragma once

#include "Texture.hpp"
#include <array>

class Cubemap : public Texture {
public:
    static std::shared_ptr<Cubemap> create(const std::string&);
    static std::shared_ptr<Cubemap> create(const std::string&, std::shared_ptr<Texture> fromTexture);
    static std::shared_ptr<Cubemap> parse(const std::string&, const std::string&);
    static std::shared_ptr<Cubemap> get(unsigned index);
    static std::shared_ptr<Cubemap> get_by_name(const std::string&);
    void load();
    void unload();
    std::shared_ptr<Texture> side(unsigned index);
    void set_side(unsigned index, std::shared_ptr<Texture>);

private:
    static std::vector<std::shared_ptr<Cubemap>> _cubemaps;
    std::array<std::weak_ptr<Texture>, 6> _sides;
    Cubemap(const std::string&);
};