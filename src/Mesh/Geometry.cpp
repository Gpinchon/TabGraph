/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:18:12
*/

#include "Mesh/Geometry.hpp"
#include "Physics/BoundingAABB.hpp" // for AABB
#include "Camera/Camera.hpp" // for Camera
#include "Material.hpp" // for Material
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Texture.hpp" // for Texture
#include "Tools.hpp"
#include "Debug.hpp"
#include "Buffer/Buffer.hpp"
#include "Buffer/BufferView.hpp"
#include "Buffer/BufferAccessor.hpp"
#include <algorithm>


//std::vector<std::shared_ptr<Geometry>> Geometry::_Geometrys;

Geometry::Geometry(const std::string &name)
    : Object(name), _bounds(new BoundingAABB(glm::vec3(0), glm::vec3(0)))
{
}

std::shared_ptr<Geometry> Geometry::Create(const std::string &name)
{
    auto vg = std::shared_ptr<Geometry>(new Geometry(name));
    return (vg);
}

Geometry::AccessorKey Geometry::GetAccessorKey(const std::string &key)
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
        BUFFER_OFFSET(byteOffset)
    );
    glCheckError();
}

void Geometry::Load()
{
    if (IsLoaded())
        return;
    for (auto accessor : _accessors) {
        if (accessor != nullptr)
            accessor->GetBufferView()->GetBuffer()->LoadToGPU();
    }
    if (Indices() != nullptr)
        Indices()->GetBufferView()->GetBuffer()->LoadToGPU();
    glCreateVertexArrays(1, &_vaoGlid);
    glBindVertexArray(_vaoGlid);
    glCheckError();
    BindAccessor(Accessor(Geometry::AccessorKey::Position),   0);
    BindAccessor(Accessor(Geometry::AccessorKey::Normal),     1);
    BindAccessor(Accessor(Geometry::AccessorKey::Tangent),    2);
    BindAccessor(Accessor(Geometry::AccessorKey::TexCoord_0), 3);
    BindAccessor(Accessor(Geometry::AccessorKey::TexCoord_1), 4);
    BindAccessor(Accessor(Geometry::AccessorKey::Color_0),    5);
    BindAccessor(Accessor(Geometry::AccessorKey::Joints_0),   6);
    BindAccessor(Accessor(Geometry::AccessorKey::Weights_0),  7);
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
    }
    else {
        auto accessor(Accessor(Geometry::AccessorKey::Position));
        auto byteOffset(accessor->ByteOffset() + accessor->GetBufferView()->ByteOffset());
        glDrawArrays(Mode(), byteOffset / accessor->TotalComponentByteSize(), accessor->Count());
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

void Geometry::SetAccessor(const Geometry::AccessorKey key, std::shared_ptr<BufferAccessor>accessor)
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
