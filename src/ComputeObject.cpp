/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 13:58:51
*/

#include "ComputeObject.hpp"
#include "Debug.hpp"
#include "Shader.hpp"

ComputeObject::ComputeObject(const std::string& name)
    : Node(name)
{
}

std::shared_ptr<ComputeObject> ComputeObject::create(const std::string& name, std::shared_ptr<Shader> computeShader)
{
    int workgroup_count[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);
    auto obj = std::shared_ptr<ComputeObject>(new ComputeObject(name));
    obj->_shader = computeShader;
    obj->_num_groups = new_vec3(workgroup_count[0], workgroup_count[1], workgroup_count[2]);
    Node::add(obj);
    return (obj);
}

std::shared_ptr<Shader> ComputeObject::shader()
{
    return (_shader.lock());
}

void ComputeObject::set_shader(std::shared_ptr<Shader> ishader)
{
    _shader = ishader;
}

void ComputeObject::load()
{
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
        glCheckError();
        shaderPtr->bind_image("out_data", outTexturePtr, 0, false, 0, GL_WRITE_ONLY, GL_TEXTURE1);
        glCheckError();
    }
    glDispatchCompute(30, 40, 1);
    glCheckError();
    glMemoryBarrier(memory_barrier());
    glCheckError();
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

VEC3 ComputeObject::num_groups()
{
    return (_num_groups);
}

void ComputeObject::set_num_groups(VEC3 groups)
{
    _num_groups = groups;
}
