/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 14:34:34
*/

#pragma once

#include "Buffer/Accessor.hpp"
#include "Buffer/View.hpp"
#include "Surface/Surface.hpp"

#include <array>
#include <glm/glm.hpp> // for glm::vec2, s_vec2, s_vec3, glm::vec3
#include <map>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Material; // lines 20-20
class Buffer::Accessor;
class BoundingAABB;

namespace Renderer {
class GeometryRenderer;
};

class GeometryMorthTarget {
public:
    enum Channel {
        Normal,
        Position,
        Tangent,
        MaxChannels
    };
    std::shared_ptr<Buffer::Accessor> Get(const GeometryMorthTarget::Channel channel) const;
    void Set(const GeometryMorthTarget::Channel channel, const std::shared_ptr<Buffer::Accessor> morphChannel);

private:
    std::array<std::shared_ptr<Buffer::Accessor>, GeometryMorthTarget::MaxChannels> _morphChannels;
};

class Geometry : public Surface {
public:
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
    enum class DrawingMode {
        Unknown = -1,
        Points,
        Lines,
        LineStrip,
        LineLoop,
        Polygon,
        Triangles,
        TriangleStrip,
        TriangleFan,
        Quads,
        QuadStrip,
        MaxValue
    };
    /** @brief Drawing mode for this geometry, default : GL_TRIANGLES */
    PROPERTY(DrawingMode, DrawingMode, DrawingMode::Triangles);
    PROPERTY(bool, Loaded, false);

public:
    Geometry();
    Geometry(const std::string& name);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<uint32_t> indices, Buffer::View::Mode = Buffer::View::Mode::Immutable);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, Buffer::View::Mode = Buffer::View::Mode::Immutable);
    Geometry(const Geometry& other);
    ~Geometry();

    static Geometry::AccessorKey GetAccessorKey(const std::string& key);
    glm::vec3 Centroid() const;
    size_t EdgeCount() const;
    glm::ivec2 GetEdge(const size_t index) const;
    size_t VertexCount() const;
    template <typename T>
    T GetVertex(const Geometry::AccessorKey key, const size_t index) const;
    /** @return : The accessor corresponding to the key */
    std::shared_ptr<Buffer::Accessor> Accessor(const Geometry::AccessorKey key) const;
    /** Sets the accessor corresponding to the key */
    void SetAccessor(const Geometry::AccessorKey key, std::shared_ptr<Buffer::Accessor>);
    std::shared_ptr<Buffer::Accessor> Indices() const;
    void SetIndices(std::shared_ptr<Buffer::Accessor>);

    std::shared_ptr<BoundingAABB> GetBounds() const;

    GeometryMorthTarget& GetMorphTarget(size_t index);
    void SetMorphTarget(const GeometryMorthTarget& morphTarget);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Geometry>(*this);
    }
    glm::vec3 _centroid { 0 };
    std::shared_ptr<BoundingAABB> _bounds;
    std::array<std::shared_ptr<Buffer::Accessor>, size_t(Geometry::AccessorKey::MaxAccessorKey)> _accessors;
    std::shared_ptr<Buffer::Accessor> _indices { nullptr };
    std::vector<GeometryMorthTarget> _morphTargets;

    // Hérité via Surface
    virtual Type GetType() const override
    {
        return Type::Geometry;
    }
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