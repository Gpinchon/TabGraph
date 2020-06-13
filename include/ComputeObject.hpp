/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-12 11:43:11
*/

#pragma once

#include "Node.hpp" // for Node
#include "glm/glm.hpp" // for glm::vec3, s_vec3
#include <GL/glew.h> // for GLbitfield, GL_ALL_BARRIER_BITS
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Shader; // lines 12-12
class Texture; // lines 13-13

class ComputeObject : public Node {
public:
    static std::shared_ptr<ComputeObject> Create(const std::string& name, std::shared_ptr<Shader> computeShader = nullptr);
    virtual std::shared_ptr<Shader> shader();
    virtual std::shared_ptr<Texture> in_texture();
    virtual std::shared_ptr<Texture> out_texture();
    virtual void set_in_texture(std::shared_ptr<Texture>);
    virtual void set_out_texture(std::shared_ptr<Texture>);
    virtual void set_shader(std::shared_ptr<Shader>);
    virtual void LoadCPU() override {};
    virtual void UnloadCPU() override {};
    virtual void LoadGPU() override {};
    virtual void UnloadGPU() override {};
    virtual void UpdateCPU() override {};
    virtual void UpdateGPU() override {};
    //virtual void load();
    virtual void run();
    glm::ivec3 num_groups();
    GLbitfield memory_barrier();
    void set_num_groups(glm::ivec3);
    void set_memory_barrier(GLbitfield);

protected:
    std::vector<std::shared_ptr<ComputeObject>> _compute_objects;
    std::weak_ptr<Texture> _in_texture;
    std::weak_ptr<Texture> _out_texture;
    std::weak_ptr<Shader> _shader;
    GLbitfield _memory_barrier { GL_ALL_BARRIER_BITS };
    glm::ivec3 _num_groups { 0, 0, 0 };
    ComputeObject(const std::string& name);
};