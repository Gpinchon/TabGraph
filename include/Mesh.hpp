/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-08-13 17:53:35
*/

#pragma once

#include "Renderable.hpp" // for RenderAll, RenderMod, Renderable
#include <GL/glew.h> // for GLenum, GL_BACK
#include <memory> // for shared_ptr, weak_ptr
#include <set>
#include <string> // for string
#include <vector> // for vector

class Vgroup;

class Mesh : public Renderable
{
public:
    static std::shared_ptr<Mesh> Create(const std::string &);
    static std::shared_ptr<Mesh> GetByName(const std::string &);
    static std::shared_ptr<Mesh> GetById(int64_t id);
    static std::shared_ptr<Mesh> Get(unsigned index);
    static void Add(std::shared_ptr<Mesh>);
    void Load() override;
    bool Draw(RenderMod mod = RenderAll) override;
    bool DrawDepth(RenderMod mod = RenderAll) override;
    bool Drawable() const override;
    void center();
    void set_cull_mod(GLenum);
    void Add(std::shared_ptr<Vgroup>);
    //std::shared_ptr<Vgroup> vgroup(unsigned index);

protected:
    Mesh(const std::string &name);

private:
    static std::vector<std::shared_ptr<Mesh>> _meshes;
    std::set<std::weak_ptr<Vgroup>> _vgroups;
    GLenum _cull_mod{GL_BACK};
};

bool alpha_compare(Renderable *m, Renderable *m1);
