/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-09 17:48:25
*/

#pragma once

#include <GL/glew.h> // for GLenum, GL_BACK
#include <memory> // for shared_ptr, weak_ptr
#include <set>
#include <string> // for string
#include <vector> // for vector
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include "Node.hpp"

class Geometry;
class MeshSkin;
class Material;
class TextureBuffer;

class Mesh : public Node
{
public:
    static std::shared_ptr<Mesh> Create(std::shared_ptr<Mesh> otherMesh);
    static std::shared_ptr<Mesh> Create(const std::string &);
    void Load() override;
    bool Draw(RenderMod mod = RenderMod::RenderAll) override;
    bool DrawDepth(RenderMod mod = RenderMod::RenderAll) override;
    bool Drawable() const override;
    void center();
    void set_cull_mod(GLenum);
    /** Adds the Geometry to Geometrys list */
    void AddGeometry(std::shared_ptr<Geometry>);
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

    /** @return a position offset not inherited by children */
    virtual glm::vec3 GeometryPosition() const;
    /** @argument position : Geometrys position offset, not inherited by children */
    virtual void SetGeometryPosition(glm::vec3 position);
    /** @return a rotation offset not inherited by children */
    virtual glm::quat GeometryRotation() const;
    /** @argument rotation : Geometrys rotation offset, not inherited by children */
    virtual void SetGeometryRotation(glm::quat rotation);
    /** @return a scaling offset not inherited by children */
    virtual glm::vec3 GeometryScale() const;
    /** @argument scale : Geometrys scale offset, not inherited by children */
    virtual void SetGeometryScale(glm::vec3 scale);

    std::shared_ptr<MeshSkin> Skin() const;
    void SetSkin(std::shared_ptr<MeshSkin> skin);

    const std::set<std::shared_ptr<Geometry>> Geometrys();

protected:
    Mesh(const std::string &name);

private:
    std::set<std::shared_ptr<Geometry>> _Geometrys;
    std::vector<std::shared_ptr<Material>> _materials;
    std::shared_ptr<MeshSkin> _skin { nullptr };
    std::shared_ptr<TextureBuffer> _jointMatrices { nullptr };
    GLenum _cull_mod{GL_BACK};
    glm::vec3 _geometryPosition{0};
    glm::quat _geometryRotation{0, 0, 0, 1};
    glm::vec3 _geometryScale{1};
    bool _loaded {false};
};
