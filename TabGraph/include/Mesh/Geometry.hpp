/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:24
*/

#pragma once

#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferView.hpp"
#include "Node.hpp" // for RenderAll, RenderMod, Renderable
#include "Object.hpp"

#include <GL/glew.h> // for GLubyte
#include <array>
#include <glm/glm.hpp> // for glm::vec2, s_vec2, s_vec3, glm::vec3
#include <map>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

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
public:
    enum class DrawingMode : GLenum {
        Unknown = -1,
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        LineLoop = GL_LINE_LOOP,
        Polygon = GL_POLYGON,
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN,
        Quads = GL_QUADS,
        QuadStrip = GL_QUAD_STRIP
    };
    /** @brief Drawing mode for this geometry, default : GL_TRIANGLES */
    PROPERTY(DrawingMode, DrawingMode, DrawingMode::Triangles);
    PROPERTY(bool, Loaded, false);

public:
    Geometry();
    Geometry(const std::string& name);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<uint32_t> indices, BufferView::Mode = BufferView::Mode::Immutable);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, BufferView::Mode = BufferView::Mode::Immutable);
    //Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, BufferView::Mode = BufferView::Mode::Immutable);
    //Geometry(const std::vector<glm::vec3>& vertices, BufferView::Mode = BufferView::Mode::Immutable);
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

    virtual void Load();

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Geometry>(*this);
    }
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
        auto indice(Indices()->Get<unsigned>(index));
        return Accessor(key)->Get<T>(indice);
    } else
        return Accessor(key)->Get<T>(index);
}