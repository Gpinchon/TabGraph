/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:27:46
*/

#pragma once

#include "Renderable.hpp"

class Vgroup;

class Mesh : public Renderable {
public:
    static std::shared_ptr<Mesh> create(const std::string&);
    static std::shared_ptr<Mesh> get_by_name(const std::string&);
    static std::shared_ptr<Mesh> get(unsigned index);
    static void add(std::shared_ptr<Mesh>);
    void load();
    bool render(RenderMod mod = RenderAll);
    bool render_depth(RenderMod mod = RenderAll);
    void center();
    void set_cull_mod(GLenum);
    void add(std::shared_ptr<Vgroup>);
    std::shared_ptr<Vgroup> vgroup(unsigned index);

protected:
    Mesh(const std::string& name);

private:
    static std::vector<std::shared_ptr<Mesh>> _meshes;
    std::vector<std::weak_ptr<Vgroup>> _vgroups;
    GLenum _cull_mod{ GL_BACK };
};

bool alpha_compare(Renderable* m, Renderable* m1);
