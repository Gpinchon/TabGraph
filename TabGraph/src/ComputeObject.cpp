/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-12 11:43:16
*/

#include "ComputeObject.hpp"
#include "Debug.hpp"
#include "Shader/Shader.hpp"

ComputeObject::ComputeObject(const std::string& name, std::shared_ptr<Shader> computeShader)
    : Node(name)
{
    int workgroup_count[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);
    _shader = computeShader;
    _num_groups = glm::vec3(workgroup_count[0], workgroup_count[1], workgroup_count[2]);
}

std::shared_ptr<Shader> ComputeObject::shader()
{
    return (_shader.lock());
}

void ComputeObject::set_shader(std::shared_ptr<Shader> ishader)
{
    _shader = ishader;
}

void ComputeObject::run()
{
    auto shaderPtr = shader();
    auto inTexturePtr = in_texture();
    auto outTexturePtr = out_texture();
    if (shaderPtr == nullptr)
        return;
    shaderPtr->use();
    if (inTexturePtr == outTexturePtr) {
        shaderPtr->bind_image("inout_data", inTexturePtr, 0, false, 0, GL_READ_WRITE, GL_TEXTURE0);
    } else {
        shaderPtr->bind_image("in_data", inTexturePtr, 0, false, 0, GL_READ_ONLY, GL_TEXTURE0);
        shaderPtr->bind_image("out_data", outTexturePtr, 0, false, 0, GL_WRITE_ONLY, GL_TEXTURE1);
    }
    glDispatchCompute(30, 40, 1);
    glMemoryBarrier(memory_barrier());
    shaderPtr->use(false);
}

std::shared_ptr<Texture> ComputeObject::out_texture()
{
    return (_out_texture.lock());
}

void ComputeObject::set_out_texture(std::shared_ptr<Texture> itexture)
{
    _out_texture = itexture;
}

std::shared_ptr<Texture> ComputeObject::in_texture()
{
    return (_in_texture.lock());
}

void ComputeObject::set_in_texture(std::shared_ptr<Texture> itexture)
{
    _in_texture = itexture;
}

GLbitfield ComputeObject::memory_barrier()
{
    return (_memory_barrier);
}

void ComputeObject::set_memory_barrier(GLbitfield barrier)
{
    _memory_barrier = barrier;
}

glm::ivec3 ComputeObject::num_groups()
{
    return (_num_groups);
}

void ComputeObject::set_num_groups(glm::ivec3 groups)
{
    _num_groups = groups;
}
