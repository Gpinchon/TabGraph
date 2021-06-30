/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 16:30:16
*/

#include <Surface/Geometry.hpp>
#include <Assets/Asset.hpp>
#include <Assets/BinaryData.hpp>
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Camera/Camera.hpp>
#include <Debug.hpp>
#include <Physics/BoundingAABB.hpp>
#include <Tools/Tools.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Surface/GeometryRenderer.hpp>
#endif

#include <algorithm>

static size_t geometryNbr(0);

Geometry::Geometry(const std::string& name)
    : Surface(name)
    , _bounds(Component::Create<BoundingAABB>(glm::vec3(0), glm::vec3(0)))
{
    _renderer.reset(new Renderer::GeometryRenderer(*this));
    geometryNbr++;
}

Geometry::Geometry()
    : Geometry("Geometry_" + std::to_string(geometryNbr))
{
}

Geometry::Geometry(const Geometry& other)
    : Surface(other)
{
    _renderer.reset(new Renderer::GeometryRenderer(*this));
    _centroid = other._centroid;
    _bounds = std::static_pointer_cast<BoundingAABB>(other._bounds->Clone());
    _accessors = other._accessors;
    _indices = other._indices;
    _morphTargets = other._morphTargets;
}

Geometry::~Geometry()
{
}

Geometry::Geometry(
    const std::vector<glm::vec3>& vertices,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoords,
    const std::vector<uint32_t> indices,
    Buffer::View::Mode mode)
    : Geometry()
{
    assert(vertices.size() == normals.size());
    assert(normals.size() == texCoords.size());
    auto vertexBuffer {
        Component::Create<BinaryData>(
            vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3) + texCoords.size() * sizeof(glm::vec2) + indices.size() * sizeof(uint32_t))
    };
    auto verticeByteSize { vertices.size() * sizeof(glm::vec3) };
    auto normalsByteSize { normals.size() * sizeof(glm::vec3) };
    auto texcoordByteSize { texCoords.size() * sizeof(glm::vec2) };
    auto indiceByteSize { indices.size() * sizeof(uint32_t) };
    vertexBuffer->Set(
        (std::byte*)vertices.data(),
        0,
        verticeByteSize);
    vertexBuffer->Set(
        (std::byte*)normals.data(),
        verticeByteSize,
        normalsByteSize);
    vertexBuffer->Set(
        (std::byte*)texCoords.data(),
        verticeByteSize + normalsByteSize,
        texcoordByteSize);
    vertexBuffer->Set(
        (std::byte*)indices.data(),
        verticeByteSize + normalsByteSize + texcoordByteSize,
        indiceByteSize);
    auto vertexBufferAsset { Component::Create<Asset>() };
    vertexBufferAsset->SetComponent(vertexBuffer);
    vertexBufferAsset->SetLoaded(true);
    auto vertexBufferView { Component::Create<Buffer::View>(vertexBufferAsset, mode) };
    vertexBufferView->SetType(Buffer::View::Type::Array);
    vertexBufferView->SetByteLength(verticeByteSize + normalsByteSize + texcoordByteSize);

    auto vertexAccessor { Component::Create<Buffer::Accessor>(Buffer::Accessor::ComponentType::Float32, Buffer::Accessor::Type::Vec3, vertexBufferView) };
    auto normalAccessor { Component::Create<Buffer::Accessor>(Buffer::Accessor::ComponentType::Float32, Buffer::Accessor::Type::Vec3, vertexBufferView) };
    auto texcoordAccessor { Component::Create<Buffer::Accessor>(Buffer::Accessor::ComponentType::Float32, Buffer::Accessor::Type::Vec2, vertexBufferView) };

    vertexAccessor->SetByteOffset(0);
    vertexAccessor->SetCount(vertices.size());
    normalAccessor->SetByteOffset(verticeByteSize);
    normalAccessor->SetCount(normals.size());
    texcoordAccessor->SetByteOffset(verticeByteSize + normalsByteSize);
    texcoordAccessor->SetCount(texCoords.size());
    normalAccessor->SetNormalized(true);
    SetAccessor(Geometry::AccessorKey::Position, vertexAccessor);
    SetAccessor(Geometry::AccessorKey::Normal, normalAccessor);
    SetAccessor(Geometry::AccessorKey::TexCoord_0, texcoordAccessor);

    if (indices.empty())
        return;
    auto indiceBufferView { Component::Create<Buffer::View>(vertexBufferAsset, mode) };
    indiceBufferView->SetType(Buffer::View::Type::ElementArray);
    indiceBufferView->SetByteLength(indiceByteSize);
    indiceBufferView->SetByteOffset(verticeByteSize + normalsByteSize + texcoordByteSize);
    auto indiceAccessor { Component::Create<Buffer::Accessor>(Buffer::Accessor::ComponentType::Uint32, Buffer::Accessor::Type::Scalar, indiceBufferView) };
    SetIndices(indiceAccessor);
}

Geometry::Geometry(
    const std::vector<glm::vec3>& vertices,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoords,
    Buffer::View::Mode mode)
    : Geometry(
        vertices,
        normals,
        texCoords,
        {}, //empty indices
        mode)
{
}

Geometry::AccessorKey Geometry::GetAccessorKey(const std::string& key)
{
    std::string lowerKey(key);
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    if (lowerKey == "position")
        return Geometry::AccessorKey::Position;
    else if (lowerKey == "normal")
        return Geometry::AccessorKey::Normal;
    else if (lowerKey == "tangent")
        return Geometry::AccessorKey::Tangent;
    else if (lowerKey == "texcoord_0")
        return Geometry::AccessorKey::TexCoord_0;
    else if (lowerKey == "texcoord_1")
        return Geometry::AccessorKey::TexCoord_1;
    else if (lowerKey == "texcoord_2")
        return Geometry::AccessorKey::TexCoord_2;
    else if (lowerKey == "color_0")
        return Geometry::AccessorKey::Color_0;
    else if (lowerKey == "joints_0")
        return Geometry::AccessorKey::Joints_0;
    else if (lowerKey == "weights_0")
        return Geometry::AccessorKey::Weights_0;
    return Geometry::AccessorKey::Invalid;
}

std::shared_ptr<Buffer::Accessor> Geometry::Accessor(const Geometry::AccessorKey key) const
{
    return _accessors.at(size_t(key));
}

void Geometry::SetAccessor(const Geometry::AccessorKey key, std::shared_ptr<Buffer::Accessor> accessor)
{
    if (key != Geometry::AccessorKey::Invalid)
        _accessors.at(size_t(key)) = accessor;
}

std::shared_ptr<Buffer::Accessor> Geometry::Indices() const
{
    return _indices;
}

void Geometry::SetIndices(std::shared_ptr<Buffer::Accessor> indices)
{
    _indices = indices;
}

std::shared_ptr<BoundingAABB> Geometry::GetBounds() const
{
    return _bounds;
}

size_t Geometry::EdgeCount() const
{
    switch (GetDrawingMode()) {
    case DrawingMode::Points:
        return 0;
    case DrawingMode::Lines:
        return VertexCount() / 2;
    case DrawingMode::LineStrip:
        return VertexCount() - 1;
    case DrawingMode::LineLoop:
    case DrawingMode::Polygon:
        return VertexCount();
    case DrawingMode::Triangles:
        return VertexCount() / 3;
    case DrawingMode::TriangleStrip:
    case DrawingMode::TriangleFan:
        return VertexCount() - (VertexCount() / 3) + 1;
    case DrawingMode::Quads:
        return VertexCount() / 4;
    case DrawingMode::QuadStrip:
        return VertexCount() - (VertexCount() / 4) + 1;
    }
    return 0;
}

/*
** GL_TRIANGLES
**  0 = 0 ; 1
**  1 = 1 ; 2
**  2 = 2 ; 0
**  3 = 3 ; 4
**  4 = 4 ; 5
**  5 = 5 ; 3
**  for edge B :
**   Get triangle "global" index (index / 3)
**   Multiply it by 3 to get correct offset
**   Add "local" triangle offset : (index + 1) % 3
** GL_TRIANGLE_STRIP
**  face = i / 4
**  A = (face * 2 + 1 + !((i + 2) % 4)) * (i > 0)
**  B = face * 2 + 2 * (i % 2) + !((i + 1) % 4)
**  face = (i - 1) / 2
**  A = face + 2 * !(i % 2) + !(face % 2) * (i % 2);
**  B = face + 2 * (i % 2) + (face % 2) * !(i % 2);

**  0 = 0 ; 1
**  face = 0
**  1 = 1 ; 2   a = face + 1 ; b = face + 2 //i impair  face pair
**  2 = 2 ; 0   a = face + 2 ; b = face + 0 //i pair    face pair
**  face = 1
**  3 = 1 ; 3   a = face + 0 ; b = face + 2 //i impair  face impair
**  4 = 3 ; 2   a = face + 2 ; b = face + 1 //i pair    face impair
**  face = 2
**  5 = 3 ; 4   a = face + 1 ; b = face + 2
**  6 = 4 ; 2   a = face + 2 ; b = face + 0
**  face = 3
**  7 = 3 ; 5   a = face + 0 ; b = face + 2
**  8 = 5 ; 4   a = face + 2 ; b = face + 1
**  face = 4
**  9 = 5 ; 6   a = face + 1 ; b = face + 2
** 10 = 6 ; 4   a = face + 2 ; b = face + 0

** GL_TRIANGLE_FAN

**  0 = 0 ; 1
**  face = 0
**  1 = 1 ; 2   a = face + 1 ; b = face + 2
**  2 = 2 ; 0   a = face + 2 ; b = 0
**  face = 1
**  3 = 2 ; 3   a = face + 1
**  4 = 3 ; 0   a = face + 2
**  face = 2
**  5 = 3 ; 4
**  6 = 4 ; 0

**  for edge A :
**   determin if index is odd (index % 2)
**   if index is odd, edge A equals 0
**   else edge A equals (index + 1) / 2
**   edge B equals index + 1 if index is not odd

**  GL_QUAD_STRIP
**  face = (i - 1) / 3
**  iMod3 = i % 3;
**  A = face * 2 + index % 3 + ((index + 2) % 3);
**  B = face + 2 + (3 - (index % 3)) % 3;
**      0 => 0 ; 1
**     face = 0;
**      1 => 1 ; 3  a = face * 2 + 1 + 0 ; face * 2 + 2 + 1
**      2 => 3 ; 2  a = face * 2 + 2 + 1 ; face * 2 + 1 + 1
**      3 => 2 ; 0  a = face * 2 + 0 + 2 ; face * 2 + 0 + 0
**     face = 1
**      4 => 3 ; 5  a = face * 2 + 1 + 0 ; face * 2 + 2 + 1
**      5 => 5 ; 4  a = face * 2 + 2 + 1 ; face * 2 + 1 + 1
**      6 => 4 ; 2  a = face * 2 + 0 + 2 ; face * 2 + 0 + 0
**     face = 2
**      7 => 5 ; 7  a = face * 2 + 1
**      8 => 7 ; 6  a = face * 2 + 3
**      9 => 6 ; 4  a = face * 2 + 2
*/

glm::ivec2 Geometry::GetEdge(const size_t index) const
{
    glm::ivec2 ret { -1 };
    switch (GetDrawingMode()) {
    case DrawingMode::Points:
        return glm::ivec2(index);
    case DrawingMode::Lines:
        return glm::ivec2(
            index * 2 + 0,
            index * 2 + 1);
    case DrawingMode::LineStrip:
        return glm::ivec2(
            index + 0,
            index + 1);
    case DrawingMode::LineLoop:
    case DrawingMode::Polygon:
        return glm::ivec2(
            index,
            (index + 1) % VertexCount());
    case DrawingMode::Triangles:
        return glm::ivec2(
            index,
            index / 3 * 3 + (index + 1) % 3);
    case DrawingMode::TriangleStrip: {
        auto face = (index - 1) / 2;
        auto zeroIndex = index == 0;
        auto oddIndex = index % 2;
        auto oddFace = face % 2;
        auto A = face + 2 * !oddIndex + !oddFace * oddIndex;
        auto B = face + 2 * oddIndex + oddFace * !oddIndex;
        A *= !zeroIndex;
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    case DrawingMode::TriangleFan: {
        auto face = (index - 1) / 2;
        auto zeroIndex = index == 0;
        auto oddIndex = (index % 2);
        auto A = face + 2 - oddIndex;
        auto B = (face + 2) * oddIndex;
        A *= !zeroIndex;
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    case DrawingMode::Quads:
        return glm::ivec2(
            index,
            index / 4 * 4 + (index + 1) % 4);
    case DrawingMode::QuadStrip: {
        int iMod3 = index % 3;
        int iIs3 = iMod3 == 0;
        int face = (index - 1) / 3;
        int zeroIndex = index == 0;
        int A = face * 2 + (iMod3) + ((index + 2) % 3);
        int B = face * 2 + 3 * !iIs3 - iMod3 + 1 * !iIs3;
        A *= !zeroIndex;
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    }
    return ret;
}

size_t Geometry::VertexCount() const
{
    return Indices() ? Indices()->GetCount() : Accessor(AccessorKey::Position)->GetCount();
}

/*void Geometry::center(glm::vec3 &center)
{
    for (auto &vec : v)
    {
        vec = vec - center;
    }
    boundingElement->min = boundingElement->min - center;
    boundingElement->max = boundingElement->max - center;
    boundingElement->center = boundingElement->center - center;
    SetPosition(boundingElement->center - center);
}*/
