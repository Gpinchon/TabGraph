/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-10 01:17:40
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Core/Inherit.hpp>
#include <Shapes/Shape.hpp>

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
namespace Renderer {
    class GeometryRenderer;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
/*class GeometryMorthTarget {
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
};*/

class Geometry : public Core::Inherit<Shape, Geometry> {
public:
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
    size_t EdgeCount() const;
    glm::ivec2 GetEdge(const size_t index) const;
    size_t VertexCount() const;

    void SetIndices(const Buffer::Accessor& accessor)
    {
        _Indices = accessor;
    }
    auto& GetIndices() const
    {
        return _Indices;
    }
    void SetJoints(const Buffer::Accessor& accessor)
    {
        _Joints = accessor;
    }
    auto& GetJoints() const
    {
        return _Joints;
    }
    void SetPositions(const Buffer::Accessor& accessor)
    {
        _Positions = accessor;
    }
    auto& GetPositions() const
    {
        return _Positions;
    }
    void SetNormals(const Buffer::Accessor& accessor)
    {
        _Normals = accessor;
    }
    auto& GetNormals() const
    {
        return _Normals;
    }
    void SetTexCoord0(const Buffer::Accessor& accessor)
    {
        _TexCoord0 = accessor;
    }
    auto& GetTexCoord0() const
    {
        return _TexCoord0;
    }
    void SetTexCoord1(const Buffer::Accessor& accessor)
    {
        _TexCoord1 = accessor;
    }
    auto& GetTexCoord1() const
    {
        return _TexCoord1;
    }
    void SetTexCoord2(const Buffer::Accessor& accessor)
    {
        _TexCoord2 = accessor;
    }
    auto& GetTexCoord2() const
    {
        return _TexCoord2;
    }

private:
    Buffer::Accessor _Indices;
    Buffer::Accessor _Joints;
    Buffer::Accessor _Positions;
    Buffer::Accessor _Normals;
    Buffer::Accessor _TexCoord0;
    Buffer::Accessor _TexCoord1;
    Buffer::Accessor _TexCoord2;
};
}
