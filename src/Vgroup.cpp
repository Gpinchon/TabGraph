/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-07 11:38:13
*/

#include "Vgroup.hpp"
#include "AABB.hpp" // for AABB
#include "BoundingElement.hpp" // for BoundingElement
#include "Camera.hpp" // for Camera
#include "Material.hpp" // for Material
#include "Shader.hpp" // for Shader
#include "Texture.hpp" // for Texture
#include "VertexArray.hpp" // for VertexArray
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"


//std::vector<std::shared_ptr<Vgroup>> Vgroup::_vgroups;

Vgroup::Vgroup(const std::string &name)
    : Object(name)
{
    boundingElement = new AABB;
}

std::shared_ptr<Vgroup> Vgroup::Create(const std::string &name)
{
    auto vg = std::shared_ptr<Vgroup>(new Vgroup(name));
    return (vg);
}

#define BUFFER_OFFSET(i) (reinterpret_cast<const char*>(i))

static inline void BindAccessor(std::shared_ptr<BufferAccessor> accessor, int index)
{
    if (accessor == nullptr)
        return;
    auto bufferView(accessor->GetBufferView());
    auto buffer(bufferView->GetBuffer());
    auto byteOffset(accessor->ByteOffset() + bufferView->ByteOffset());
    glBindBuffer(bufferView->Target(), buffer->Glid());
    glCheckError();
    glEnableVertexAttribArray(index);
    glCheckError();
    glVertexAttribPointer(
        index,
        accessor->ComponentSize(),
        accessor->ComponentType(),
        accessor->Normalized(),
        bufferView->ByteStride(),
        BUFFER_OFFSET(byteOffset * accessor->ComponentByteSize())
        );
    glCheckError();
}

void Vgroup::Load()
{
    if (IsLoaded())
    {
        return;
    }
    for (auto accessor : _accessors)
        accessor.second->Load();
    Indices()->Load();
    glGenVertexArrays(1, &_vaoGlid);
    glCheckError();
    glBindVertexArray(_vaoGlid);
    glCheckError();
    BindAccessor(Accessor("POSITION"), 0);
    BindAccessor(Accessor("NORMAL"), 1);
    BindAccessor(Accessor("TEXCOORD_0"), 2);
    glBindVertexArray(0);
    //BindAccessor(Accessor("INDICES"), 3);

    /*_vao = VertexArray::Create(v.size());
    auto vaoPtr = _vao.lock();
    vaoPtr->add_buffer(GL_FLOAT, 3, v);
    vaoPtr->add_buffer(GL_UNSIGNED_BYTE, 4, vn);
    vaoPtr->add_buffer(GL_FLOAT, 2, vt);
    vaoPtr->add_indices(i);*/
    _isLoaded = true;
}

bool Vgroup::IsLoaded() const
{
    return _isLoaded;
}

bool Vgroup::Draw()
{
    /*auto vaoPtr = _vao.lock();
    if (nullptr == vaoPtr)
        return (false);
    vaoPtr->draw();*/
    Load();
    glBindVertexArray(_vaoGlid);
    std::cout << _indices->Count() << std::endl;
    if (_indices != nullptr) {
        /*for (auto data : _indices->GetBufferView()->GetBuffer()->RawData())
            std::cout << std::to_integer<int>(data) << " ";
        std::cout << std::endl;*/
        glDrawElements(Mode(), _indices->Count(), _indices->ComponentType(), &_indices->GetBufferView()->GetBuffer()->RawData().at(0));
    }
    else
        glDrawArrays(Mode(), 0, Accessor("POSITION")->Count());
    glBindVertexArray(0);

    return (true);
}

uint32_t Vgroup::MaterialIndex()
{
    return _materialIndex;
}
void Vgroup::SetMaterialIndex(uint32_t index)
{
    _materialIndex = index;
}

GLenum Vgroup::Mode() const
{
    return _drawingMode;
}

void Vgroup::SetMode(GLenum drawingMode)
{
    _drawingMode = drawingMode;
}

std::shared_ptr<BufferAccessor> Vgroup::Accessor(const std::string &key) const
{
    auto accessor(_accessors.find(key));
    if (accessor != _accessors.end())
        return _accessors.find(key)->second;
    return nullptr;
}

void Vgroup::SetAccessor(const std::string &key, std::shared_ptr<BufferAccessor>accessor)
{
    _accessors[key] = accessor;
}

std::shared_ptr<BufferAccessor> Vgroup::Indices() const
{
    return _indices;
}

void Vgroup::SetIndices(std::shared_ptr<BufferAccessor> indices)
{
    _indices = indices;
}


/*void Vgroup::center(glm::vec3 &center)
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

/*void Vgroup::set_material(std::shared_ptr<Material> mtl)
{
    if (mtl != nullptr)
        _material = mtl;
    else
        _material.reset();
}

std::shared_ptr<Material> Vgroup::material()
{
    return (_material.lock());
}*/
