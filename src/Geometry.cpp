/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-07 11:38:13
*/

#include "Geometry.hpp"
#include "AABB.hpp" // for AABB
#include "BoundingElement.hpp" // for BoundingElement
#include "Camera.hpp" // for Camera
#include "Material.hpp" // for Material
#include "Shader.hpp" // for Shader
#include "Texture.hpp" // for Texture
#include "Tools.hpp"
#include "Debug.hpp"
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"


//std::vector<std::shared_ptr<Geometry>> Geometry::_Geometrys;

Geometry::Geometry(const std::string &name)
    : Object(name)
{
    boundingElement = new AABB;
}

std::shared_ptr<Geometry> Geometry::Create(const std::string &name)
{
    auto vg = std::shared_ptr<Geometry>(new Geometry(name));
    return (vg);
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
        accessor.second->GetBufferView()->GetBuffer()->LoadToGPU();
        //accessor.second->GetBufferView()->GetBuffer()->UnloadFromCPU();
    }
    if (Indices() != nullptr)
        Indices()->GetBufferView()->GetBuffer()->LoadToGPU();
    glCreateVertexArrays(1, &_vaoGlid);
    glBindVertexArray(_vaoGlid);
    glCheckError();
    BindAccessor(Accessor("POSITION"),      0);
    BindAccessor(Accessor("NORMAL"),        1);
    BindAccessor(Accessor("TEXCOORD_0"),    2);
    BindAccessor(Accessor("TEXCOORD_1"),    3);
    BindAccessor(Accessor("COLOR_0"),       4);
    BindAccessor(Accessor("JOINTS_0"),      5);
    BindAccessor(Accessor("WEIGHTS_0"),     6);
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
        auto accessor(Accessor("POSITION"));
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

std::shared_ptr<BufferAccessor> Geometry::Accessor(const std::string &key) const
{
    auto accessor(_accessors.find(key));
    if (accessor != _accessors.end())
        return _accessors.find(key)->second;
    return nullptr;
}

void Geometry::SetAccessor(const std::string &key, std::shared_ptr<BufferAccessor>accessor)
{
    _accessors[key] = accessor;
}

std::shared_ptr<BufferAccessor> Geometry::Indices() const
{
    return _indices;
}

void Geometry::SetIndices(std::shared_ptr<BufferAccessor> indices)
{
    _indices = indices;
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
