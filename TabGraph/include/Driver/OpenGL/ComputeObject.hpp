/*
* @Author: gpinchon
* @Date:   2021-05-18 15:42:07
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 16:45:50
*/

#pragma once

#include <Property.hpp>
#include <ComputeObject.hpp>

#include <glm/vec3.hpp>

class ComputeObject::Impl {
    PROPERTY(std::shared_ptr<Shader::Program>, Shader, nullptr);
    PROPERTY(std::shared_ptr<Texture>, InputTexture, nullptr);
    PROPERTY(std::shared_ptr<Texture>, OutputTexture, nullptr);
    PROPERTY(glm::ivec3, NumGroups, 0);
    PROPERTY(ComputeObject::MemoryBarrier, MemoryBarrier, ComputeObject::MemoryBarrier::Unknown);

public:
    Impl() = delete;
    Impl(const Impl&) = delete;
    Impl(ComputeObject& computeObject);
    ~Impl();
    void Run();

private:
    ComputeObject& _computeObject;
};

namespace OpenGL {
unsigned GetBitfield(ComputeObject::MemoryBarrier);
};