/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-25 17:48:22
*/

#include "Mesh/Geometry.hpp"
#include "Buffer/Buffer.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferView.hpp"
#include "Camera/Camera.hpp" // for Camera
#include "Debug.hpp"
#include "Material.hpp" // for Material
#include "Physics/BoundingAABB.hpp" // for AABB
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Texture.hpp" // for Texture
#include "Tools.hpp"
#include <algorithm>

//std::vector<std::shared_ptr<Geometry>> Geometry::_Geometrys;

Geometry::Geometry(const std::string& name)
    : Component(name)
    , _bounds(new BoundingAABB(glm::vec3(0), glm::vec3(0)))
{
}

std::shared_ptr<Geometry> Geometry::Create(const std::string& name)
{
    auto vg = std::shared_ptr<Geometry>(new Geometry(name));
    return (vg);
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

static inline void BindAccessor(std::shared_ptr<BufferAccessor> accessor, int index)
{
    if (accessor == nullptr)
        return;
    auto bufferView(accessor->GetBufferView());
    auto buffer(bufferView->GetBuffer());
    auto byteOffset(accessor->ByteOffset() + bufferView->ByteOffset());
    glBindBuffer(GL_ARRAY_BUFFER, buffer->Glid());
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index,
        accessor->ComponentSize(),
        accessor->ComponentType(),
        accessor->Normalized(),
        bufferView->ByteStride(),
        BUFFER_OFFSET(byteOffset));
    if (glCheckError())
        throw std::runtime_error("Error while binding Accessor " + accessor->Name() + " at " + std::to_string(index));
}

void Geometry::Load()
{
    if (IsLoaded())
        return;
    debugLog(Name());
    for (auto accessor : _accessors) {
        if (accessor != nullptr)
            accessor->GetBufferView()->GetBuffer()->LoadToGPU();
    }
    if (Indices() != nullptr)
        Indices()->GetBufferView()->GetBuffer()->LoadToGPU();
    glGenVertexArrays(1, &_vaoGlid);
    glBindVertexArray(_vaoGlid);
    if (glCheckError(Name()))
        throw std::runtime_error("Error while binding VertexArray " + std::to_string(_vaoGlid));
    BindAccessor(Accessor(Geometry::AccessorKey::Position), 0);
    BindAccessor(Accessor(Geometry::AccessorKey::Normal), 1);
    BindAccessor(Accessor(Geometry::AccessorKey::Tangent), 2);
    BindAccessor(Accessor(Geometry::AccessorKey::TexCoord_0), 3);
    BindAccessor(Accessor(Geometry::AccessorKey::TexCoord_1), 4);
    BindAccessor(Accessor(Geometry::AccessorKey::Color_0), 5);
    BindAccessor(Accessor(Geometry::AccessorKey::Joints_0), 6);
    BindAccessor(Accessor(Geometry::AccessorKey::Weights_0), 7);
    glBindVertexArray(0);
    _isLoaded = true;
}

bool Geometry::IsLoaded() const
{
    return _isLoaded;
}

bool Geometry::Draw()
{
    Load();
    glBindVertexArray(_vaoGlid);
    if (Indices() != nullptr) {
        auto bufferView(Indices()->GetBufferView());
        auto byteOffset(Indices()->ByteOffset() + bufferView->ByteOffset());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferView->GetBuffer()->Glid());
        glDrawElements(Mode(), Indices()->Count(), Indices()->ComponentType(), BUFFER_OFFSET(byteOffset));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glCheckError(Name());
    } else if (auto accessor(Accessor(Geometry::AccessorKey::Position)); accessor != nullptr) {
        auto byteOffset(accessor->ByteOffset() + accessor->GetBufferView()->ByteOffset());
        glDrawArrays(Mode(), byteOffset / accessor->TotalComponentByteSize(), accessor->Count());
        glCheckError(Name());
    }
    glBindVertexArray(0);

    return (true);
}

uint32_t Geometry::MaterialIndex()
{
    return _materialIndex;
}

void Geometry::SetMaterialIndex(uint32_t index)
{
    _materialIndex = index;
}

GLenum Geometry::Mode() const
{
    return _drawingMode;
}

void Geometry::SetMode(GLenum drawingMode)
{
    _drawingMode = drawingMode;
}

std::shared_ptr<BufferAccessor> Geometry::Accessor(const Geometry::AccessorKey key) const
{
    return _accessors.at(key);
}

void Geometry::SetAccessor(const Geometry::AccessorKey key, std::shared_ptr<BufferAccessor> accessor)
{
    if (key != Geometry::AccessorKey::Invalid)
        _accessors.at(key) = accessor;
}

std::shared_ptr<BufferAccessor> Geometry::Indices() const
{
    return _indices;
}

void Geometry::SetIndices(std::shared_ptr<BufferAccessor> indices)
{
    _indices = indices;
}

std::shared_ptr<BoundingAABB> Geometry::GetBounds() const
{
    return _bounds;
}

size_t Geometry::EdgeCount() const
{
    switch (Mode()) {
    case GL_POINTS:
        return 0;
    case GL_LINES:
        return VertexCount() / 2;
    case GL_LINE_STRIP:
        return VertexCount() - 1;
    case GL_LINE_LOOP:
    case GL_POLYGON:
        return VertexCount();
    case GL_TRIANGLES:
        return VertexCount() / 3;
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
        return VertexCount() - (VertexCount() / 3) + 1;
    case GL_QUADS:
        return VertexCount() / 4;
    case GL_QUAD_STRIP:
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
    switch (Mode()) {
    case GL_POINTS:
        return glm::ivec2(index);
    case GL_LINES:
        return glm::ivec2(
            index * 2 + 0,
            index * 2 + 1);
    case GL_LINE_STRIP:
        return glm::ivec2(
            index + 0,
            index + 1);
    case GL_LINE_LOOP:
    case GL_POLYGON:
        return glm::ivec2(
            index,
            (index + 1) % VertexCount());
    case GL_TRIANGLES:
        return glm::ivec2(
            index,
            index / 3 * 3 + (index + 1) % 3);
    case GL_TRIANGLE_STRIP: {
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
    case GL_TRIANGLE_FAN: {
        auto face = (index - 1) / 2;
        auto zeroIndex = index == 0;
        auto oddIndex = (index % 2);
        auto A = face + 2 - oddIndex;
        auto B = (face + 2) * oddIndex;
        A *= !zeroIndex;
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    case GL_QUADS:
        return glm::ivec2(
            index,
            index / 4 * 4 + (index + 1) % 4);
    case GL_QUAD_STRIP: {
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
    return Indices() ? Indices()->Count() : Accessor(Position)->Count();
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

/*void Geometry::set_material(std::shared_ptr<Material> mtl)
{
    if (mtl != nullptr)
        _material = mtl;
    else
        _material.reset();
}

std::shared_ptr<Material> Geometry::material()
{
    return (_material.lock());
}*/
