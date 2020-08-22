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

typedef glm::vec<4, uint8_t, glm::defaultp> CVEC4;
/*
static inline CVEC4 VecToCVec4(glm::vec4 v) {
    v = ((v + 1.f) * 0.5f) * 255.f;
    return CVEC4(v.x, v.y, v.z, v.w);
}

static inline CVEC4 VecToCVec4(glm::vec3 v) {
    v = ((v + 1.f) * 0.5f) * 255.f;
    return CVEC4(v.x, v.y, v.z, 255);
}
*/

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
public:
    enum AccessorKey {
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
    static std::shared_ptr<Geometry> Create(const std::string& = "");
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
    /** Drawing mode */
    GLenum Mode() const;
    /** Sets the Drawing mod */
    void SetMode(GLenum drawingMode);

    std::shared_ptr<BoundingAABB> GetBounds() const;

    GeometryMorthTarget& GetMorphTarget(size_t index);
    void SetMorphTarget(const GeometryMorthTarget& morphTarget);

protected:
    Geometry(const std::string&);

private:
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
    GLenum _drawingMode { GL_TRIANGLES };
    uint32_t _materialIndex { 0 };
    GLuint _vaoGlid { 0 };
    std::array<std::shared_ptr<BufferAccessor>, Geometry::AccessorKey::MaxAccessorKey> _accessors;
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