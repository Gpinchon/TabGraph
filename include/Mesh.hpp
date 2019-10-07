/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-07 09:03:25
*/

#pragma once

#include "Renderable.hpp" // for RenderAll, RenderMod, Renderable
#include <GL/glew.h> // for GLenum, GL_BACK
#include <memory> // for shared_ptr, weak_ptr
#include <set>
#include <string> // for string
#include <vector> // for vector

class Vgroup;
class Material;

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
    /** Adds the vgroup to vgroups list */
    void AddVgroup(std::shared_ptr<Vgroup>);
    /** Adds the material to materials list */
    void AddMaterial(std::shared_ptr<Material>);
    /** Removes the material from materials list */
    void RemoveMaterial(std::shared_ptr<Material>);
    /** Sets the material at specified index */
    void SetMaterial(std::shared_ptr<Material>, uint32_t index);
    /** @return the material at specified index */
    std::shared_ptr<Material> GetMaterial(uint32_t index);
    /** @return the material index in this mesh material table, -1 if not found */
    int64_t GetMaterialIndex(std::shared_ptr<Material>);
    /** @return the material index in this mesh material table using its name, -1 if not found */
    int64_t GetMaterialIndex(const std::string &);

    const std::set<std::shared_ptr<Vgroup>> vgroups();

protected:
    Mesh(const std::string &name);

private:
    static std::vector<std::shared_ptr<Mesh>> _meshes;
    std::set<std::shared_ptr<Vgroup>> _vgroups;
    std::vector<std::shared_ptr<Material>> _materials;
    GLenum _cull_mod{GL_BACK};
};

bool alpha_compare(Renderable *m, Renderable *m1);
