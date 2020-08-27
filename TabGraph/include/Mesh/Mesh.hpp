/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 21:51:55
*/

#pragma once

#include "Component.hpp"
#include "Node.hpp"
#include <GL/glew.h> // for GLenum, GL_BACK
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <memory> // for shared_ptr, weak_ptr
#include <set>
#include <string> // for string
#include <vector> // for vector

class Geometry;
class MeshSkin;
class Material;
class TextureBuffer;
class BufferAccessor;

class Mesh : public Component {
public:
    static std::shared_ptr<Mesh> Create(std::shared_ptr<Mesh> otherMesh);
    static std::shared_ptr<Mesh> Create(const std::string&);
    static std::shared_ptr<Mesh> Create();
    bool Draw(const std::shared_ptr<Transform>& transform, RenderMod mod = RenderMod::RenderAll);
    bool DrawDepth(const std::shared_ptr<Transform>& transform, RenderMod mod = RenderMod::RenderAll);
    bool Drawable() const;
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
    int64_t GetMaterialIndex(const std::string&);

    std::shared_ptr<BufferAccessor> Weights() const;
    void SetWeights(std::shared_ptr<BufferAccessor> weights);
    const std::set<std::shared_ptr<Geometry>> Geometrys();

    virtual void UpdateSkin(const std::shared_ptr<Transform>& transform);

protected:
    Mesh(const std::string& name);

private:
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override;
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override;
    std::set<std::shared_ptr<Geometry>> _Geometrys;
    std::vector<std::shared_ptr<Material>> _materials;
    std::shared_ptr<TextureBuffer> _jointMatrices { nullptr };
    std::shared_ptr<BufferAccessor> _weights { nullptr };
    GLenum _cull_mod { GL_BACK };
};
