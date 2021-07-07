/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 14:34:34
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Shapes/Shape.hpp>
#include <Core/Inherit.hpp>

#include <array>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Buffer {
class Accessor;
}
namespace Renderer {
class GeometryRenderer;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class GeometryMorthTarget {
public:
    enum Channel {
        Normal,
        Position,
        Tangent,
        MaxChannels
    };
    inline auto Get(const GeometryMorthTarget::Channel channel) const {
        return _morphChannels.at(size_t(channel));
    }
    inline void Set(const GeometryMorthTarget::Channel channel, const std::shared_ptr<Buffer::Accessor> morphChannel) {
        _morphChannels.at(size_t(channel)) = morphChannel;
    }

private:
    std::array<std::shared_ptr<Buffer::Accessor>, GeometryMorthTarget::MaxChannels> _morphChannels;
};

class Geometry : public Core::Inherit<Shape, Geometry> {
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
    PROPERTY(glm::vec3, Centroid, 0);
    READONLYPROPERTY(bool, Loaded, false);

public:
    Geometry();
    Geometry(const std::string& name);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<uint32_t> indices, Buffer::View::Mode = Buffer::View::Mode::Immutable);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, Buffer::View::Mode = Buffer::View::Mode::Immutable);
    Geometry(const Geometry& other);
    ~Geometry();

    static Geometry::AccessorKey GetAccessorKey(const std::string& key);
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

    GeometryMorthTarget& GetMorphTarget(size_t index);
    void SetMorphTarget(const GeometryMorthTarget& morphTarget);

private:
    std::array<std::shared_ptr<Buffer::Accessor>, size_t(Geometry::AccessorKey::MaxAccessorKey)> _accessors;
    std::shared_ptr<Buffer::Accessor> _indices { nullptr };
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
}
