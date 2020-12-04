/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 21:50:39
*/

#pragma once

#include "Buffer/BufferHelper.hpp"
#include "Node.hpp" // for RenderAll, RenderMod, Renderable
#include "Object.hpp"

#include <GL/glew.h> // for GLubyte
#include <glm/glm.hpp> // for glm::vec2, s_vec2, s_vec3, glm::vec3
#include <map>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector
#include <array>

class Material; // lines 20-20
class BufferAccessor;
class BoundingAABB;

class GeometryMorthTarget {
public:
    enum Channel {
        Normal,
        Position,
        Tangent,
        MaxChannels
    };
    std::shared_ptr<BufferAccessor> Get(const GeometryMorthTarget::Channel channel) const;
    void Set(const GeometryMorthTarget::Channel channel, const std::shared_ptr<BufferAccessor> morphChannel);

private:
    std::array<std::shared_ptr<BufferAccessor>, GeometryMorthTarget::MaxChannels> _morphChannels;
};

class Geometry : public Component {
    /** @brief Drawing mode for this geometry, default : GL_TRIANGLES */
    PROPERTY(GLenum, DrawingMode, GL_TRIANGLES);
public:
    Geometry();
    Geometry(const std::string& name);
    enum class AccessorKey {
        Invalid = -1,
        Position,
        Normal,
        Tangent,
        TexCoord_0,
        TexCoord_1,
        TexCoord_2,
        Color_0,
        Joints_0,
        Weights_0,
        MaxAccessorKey
    };
    static Geometry::AccessorKey GetAccessorKey(const std::string& key);
    glm::vec3 Centroid() const;
    size_t EdgeCount() const;
    glm::ivec2 GetEdge(const size_t index) const;
    size_t VertexCount() const;
    template <typename T>
    T GetVertex(const Geometry::AccessorKey key, const size_t index) const;
    uint32_t MaterialIndex();
    void SetMaterialIndex(uint32_t);
    bool Draw();
    /** @return : The accessor corresponding to the key */
    std::shared_ptr<BufferAccessor> Accessor(const Geometry::AccessorKey key) const;
    /** Sets the accessor corresponding to the key */
    void SetAccessor(const Geometry::AccessorKey key, std::shared_ptr<BufferAccessor>);
    std::shared_ptr<BufferAccessor> Indices() const;
    void SetIndices(std::shared_ptr<BufferAccessor>);

    std::shared_ptr<BoundingAABB> GetBounds() const;

    GeometryMorthTarget& GetMorphTarget(size_t index);
    void SetMorphTarget(const GeometryMorthTarget& morphTarget);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<Geometry>(*this);
    }
    virtual void _LoadCPU() {};
    virtual void _UnloadCPU() {};
    virtual void _LoadGPU();
    virtual void _UnloadGPU() {};
    virtual void _UpdateCPU(float) {};
    virtual void _UpdateGPU(float) {};
    virtual void _FixedUpdateCPU(float) {};
    virtual void _FixedUpdateGPU(float) {};
    glm::vec3 _centroid { 0 };
    std::shared_ptr<BoundingAABB> _bounds;
    uint32_t _materialIndex { 0 };
    GLuint _vaoGlid { 0 };
    std::array<std::shared_ptr<BufferAccessor>, size_t(Geometry::AccessorKey::MaxAccessorKey)> _accessors;
    std::shared_ptr<BufferAccessor> _indices { nullptr };
    std::vector<GeometryMorthTarget> _morphTargets;
};

template <typename T>
inline T Geometry::GetVertex(const Geometry::AccessorKey key, const size_t index) const
{
    assert(index < VertexCount());
    if (Indices() != nullptr) {
        auto indice(BufferHelper::Get<unsigned>(Indices(), index));
        return BufferHelper::Get<T>(Accessor(key), indice);
    } else
        return BufferHelper::Get<T>(Accessor(key), index);
}