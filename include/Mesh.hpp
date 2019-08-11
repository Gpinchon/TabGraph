/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:09
*/

#pragma once

#include "Renderable.hpp" // for RenderAll, RenderMod, Renderable
#include <GL/glew.h> // for GLenum, GL_BACK
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Vgroup;

class Mesh : public Renderable {
public:
    static std::shared_ptr<Mesh> Create(const std::string&);
    static std::shared_ptr<Mesh> GetByName(const std::string&);
    static std::shared_ptr<Mesh> Get(unsigned index);
    static void Add(std::shared_ptr<Mesh>);
    void load() override;
    bool render(RenderMod mod = RenderAll) override;
    bool render_depth(RenderMod mod = RenderAll) override;
    void center();
    void set_cull_mod(GLenum);
    void Add(std::shared_ptr<Vgroup>);
    std::shared_ptr<Vgroup> vgroup(unsigned index);

protected:
    Mesh(const std::string& name);

private:
    static std::vector<std::shared_ptr<Mesh>> _meshes;
    std::vector<std::weak_ptr<Vgroup>> _vgroups;
    GLenum _cull_mod { GL_BACK };
};

bool alpha_compare(Renderable* m, Renderable* m1);
