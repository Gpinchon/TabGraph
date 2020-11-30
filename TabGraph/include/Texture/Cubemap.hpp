/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:15:11
*/

#pragma once

#include "Texture2D.hpp" // for Texture2D
#include <array> // for array
#include <memory> // for shared_ptr, shared_ptr
#include <string> // for string
#include <vector> // for vector
#include <filesystem>

class Cubemap : public Texture {
public:
    Cubemap(const std::string&);
    Cubemap(const std::string&, std::shared_ptr<Texture2D> fromTexture);
    ~Cubemap();
    static std::shared_ptr<Cubemap> parse(const std::filesystem::path);
    virtual void load() override;
    virtual void unload() override;
    std::shared_ptr<Texture2D> side(unsigned index);
    void set_side(unsigned index, std::shared_ptr<Texture2D>);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<Cubemap>(*this);
    }
    std::array<std::shared_ptr<Texture2D>, 6> _sides;
    
};