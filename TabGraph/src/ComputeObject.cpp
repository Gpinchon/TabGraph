/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 16:06:21
*/

#include <ComputeObject.hpp>

//#ifdef OPENGL
#include <Driver/OpenGL/ComputeObject.hpp>
//#endif

ComputeObject::ComputeObject(std::shared_ptr<Shader::Program> computeShader)
    : _impl(new ComputeObject::Impl(*this))
{
    SetShader(computeShader);
}

ComputeObject::~ComputeObject()
{
}

std::shared_ptr<Shader::Program> ComputeObject::GetShader() const
{
    return _impl->GetShader();
}

std::shared_ptr<Texture> ComputeObject::GetInputTexture() const
{
    return _impl->GetInputTexture();
}

std::shared_ptr<Texture> ComputeObject::GetOutputTexture() const
{
    return _impl->GetOutputTexture();
}

glm::ivec3 ComputeObject::GetNumGroups() const
{
    return _impl->GetNumGroups();
}

ComputeObject::MemoryBarrier ComputeObject::GetMemoryBarrier() const
{
    return _impl->GetMemoryBarrier();
}

void ComputeObject::SetShader(std::shared_ptr<Shader::Program> shader)
{
    _impl->SetShader(shader);
}

void ComputeObject::SetInputTexture(std::shared_ptr<Texture> texture)
{
    _impl->SetInputTexture(texture);
}

void ComputeObject::SetOutputTexture(std::shared_ptr<Texture> texture)
{
    _impl->SetOutputTexture(texture);
}

void ComputeObject::SetNumGroups(glm::ivec3 numGroups)
{
    _impl->SetNumGroups(numGroups);
}

void ComputeObject::SetMemoryBarrier(MemoryBarrier memoryBarrier)
{
    _impl->SetMemoryBarrier(memoryBarrier);
}

void ComputeObject::Run()
{
    _impl->Run();
}
