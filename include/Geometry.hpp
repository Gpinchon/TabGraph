/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-08 13:59:15
*/

#pragma once

#include "Object.hpp"
#include "Node.hpp" // for RenderAll, RenderMod, Renderable
#include "glm/glm.hpp" // for glm::vec2, s_vec2, s_vec3, glm::vec3
#include <GL/glew.h> // for GLubyte
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector
#include <map>

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
    enum Channel
    {
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

class Geometry : public Object
{
public:
    enum AccessorKey
    {
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
    static std::shared_ptr<Geometry> Create(const std::string & = "");
    static Geometry::AccessorKey GetAccessorKey(const std::string &key);
    uint32_t MaterialIndex();
    void SetMaterialIndex(uint32_t);
    void Load();
    bool Draw();
    bool IsLoaded() const;
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

    GeometryMorthTarget &GetMorphTarget(size_t index);
    void SetMorphTarget(const GeometryMorthTarget &morphTarget);

protected:
    Geometry(const std::string &);
private:
    bool _isLoaded { false };
    std::shared_ptr<BoundingAABB> _bounds;
    GLenum _drawingMode { GL_TRIANGLES };
    uint32_t _materialIndex { 0 };
    GLuint _vaoGlid{0};
    std::array<std::shared_ptr<BufferAccessor>, Geometry::AccessorKey::MaxAccessorKey> _accessors;
    std::shared_ptr<BufferAccessor> _indices { nullptr };
    std::vector<GeometryMorthTarget> _morphTargets;
};