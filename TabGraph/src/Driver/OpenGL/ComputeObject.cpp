/*
* @Author: gpinchon
* @Date:   2021-05-18 15:43:42
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-18 16:53:04
*/

#include <Driver/OpenGL/ComputeObject.hpp>

#include <Shader/Program.hpp>

#include <GL/glew.h>

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif //MemoryBarrier

namespace OpenGL {
unsigned GetBitfield(ComputeObject::MemoryBarrier memoryBarrier) {
    switch (memoryBarrier)
    {
    default:
        return 0;
    }
}
};

ComputeObject::Impl::Impl(ComputeObject& computeObject)
    : _computeObject(computeObject)
{
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &_NumGroups[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &_NumGroups[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &_NumGroups[2]);
}

void ComputeObject::Impl::Run()
{
    auto shaderPtr = GetShader();
    auto inTexturePtr = GetInputTexture();
    auto outTexturePtr = GetOutputTexture();
    if (shaderPtr == nullptr)
        return;
    shaderPtr->Use();
    //TODO reimplement this properly
    //if (inTexturePtr == outTexturePtr) {
    //    shaderPtr->bind_image("inout_data", inTexturePtr, 0, false, 0, GL_READ_WRITE, GL_TEXTURE0);
    //} else {
    //    shaderPtr->bind_image("in_data", inTexturePtr, 0, false, 0, GL_READ_ONLY, GL_TEXTURE0);
    //    shaderPtr->bind_image("out_data", outTexturePtr, 0, false, 0, GL_WRITE_ONLY, GL_TEXTURE1);
    //}
    glDispatchCompute(GetNumGroups().x, GetNumGroups().y, GetNumGroups().z);
    glMemoryBarrier(OpenGL::GetBitfield(GetMemoryBarrier()));
    shaderPtr->Done();
}