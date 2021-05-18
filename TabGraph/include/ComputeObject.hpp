/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 16:06:48
*/

#pragma once

#include <glm/fwd.hpp> // for ivec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string

namespace Shader {
class Program;
}
class Texture;

class ComputeObject {
public:
    enum class MemoryBarrier {
        Unknown = -1,
        MaxValue
    };
    class Impl;
    ComputeObject(std::shared_ptr<Shader::Program> computeShader = nullptr);
    ~ComputeObject();
    std::shared_ptr<Shader::Program> GetShader() const;
    std::shared_ptr<Texture> GetInputTexture() const;
    std::shared_ptr<Texture> GetOutputTexture() const;
    glm::ivec3 GetNumGroups() const;
    MemoryBarrier GetMemoryBarrier() const;
    void SetShader(std::shared_ptr<Shader::Program>);
    void SetInputTexture(std::shared_ptr<Texture>);
    void SetOutputTexture(std::shared_ptr<Texture>);
    void SetNumGroups(glm::ivec3);
    void SetMemoryBarrier(MemoryBarrier);
    void Run();

protected:
    std::unique_ptr<Impl> _impl;
};