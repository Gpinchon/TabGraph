/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:23
*/

#pragma once

#include "Component.hpp"
#include "Mesh/Geometry.hpp" // for Geometry
#include "Node.hpp"
#include "Texture/TextureBuffer.hpp"

#include <GL/glew.h> // for GLenum, GL_BACK
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <memory> // for shared_ptr, weak_ptr
#include <set>
#include <string> // for string
#include <vector> // for vector

class MeshSkin;
class Material;
class BufferAccessor;

class Mesh : public Component {
    READONLYPROPERTY(bool, Loaded, false);
public:
    Mesh();
    Mesh(const std::string& name);
    bool Draw(const std::shared_ptr<Transform>& transform, const RenderPass& pass, RenderMod mod = RenderMod::RenderAll);
    bool DrawDepth(const std::shared_ptr<Transform>& transform, RenderMod mod = RenderMod::RenderAll);
    bool Drawable() const;
    void center();
    void set_cull_mod(GLenum);
    /** Adds the Geometry to Geometrys list */
    void AddGeometry(std::shared_ptr<Geometry> geometry);
    /** Adds the material to materials list */
    void AddMaterial(std::shared_ptr<Material>);
    /** Removes the material from materials list */
    void RemoveMaterial(std::shared_ptr<Material>);
    /** @return the material at specified index */
    std::shared_ptr<Material> GetMaterial(uint32_t index);
    /** @return the material index in this mesh material table, -1 if not found */
    int64_t GetMaterialIndex(std::shared_ptr<Material>);
    /** @return the material index in this mesh material table using its name, -1 if not found */
    int64_t GetMaterialIndex(const std::string&);
    std::shared_ptr<TextureBuffer> JointMatrices() const;
    void SetJointMatrices(const std::shared_ptr<TextureBuffer>&);

    std::shared_ptr<BufferAccessor> Weights() const;
    void SetWeights(std::shared_ptr<BufferAccessor> weights);

    const auto Geometrys() const
    {
        return GetComponents<Geometry>();
    }

    virtual void Load();

    virtual void UpdateSkin(const std::shared_ptr<Transform>& transform);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        //return std::static_pointer_cast<Mesh>(shared_from_this());
        auto mesh(Component::Create<Mesh>(*this));
        return mesh;
    }
    virtual void _UpdateGPU(float /*delta*/)
    {
        _prevTransformMatrix = _transformMatrix;
    };
    GLenum _cull_mod { GL_BACK };
    glm::mat4 _transformMatrix { 1 };
    glm::mat4 _prevTransformMatrix { 1 };
};
