/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:44
*/

#pragma once

#include "Node.hpp"

class Shader;
class Texture;

class ComputeObject : public Node {
public:
    static std::shared_ptr<ComputeObject> create(const std::string& name, std::shared_ptr<Shader> computeShader = nullptr);
    virtual std::shared_ptr<Shader> shader();
    virtual std::shared_ptr<Texture> in_texture();
    virtual std::shared_ptr<Texture> out_texture();
    virtual void set_in_texture(std::shared_ptr<Texture>);
    virtual void set_out_texture(std::shared_ptr<Texture>);
    virtual void set_shader(std::shared_ptr<Shader>);
    virtual void load();
    virtual void run();
    VEC3 num_groups();
    GLbitfield memory_barrier();
    void set_num_groups(VEC3);
    void set_memory_barrier(GLbitfield);

protected:
    std::vector<std::shared_ptr<ComputeObject>> _compute_objects;
    std::weak_ptr<Texture> _in_texture;
    std::weak_ptr<Texture> _out_texture;
    std::weak_ptr<Shader> _shader;
    GLbitfield _memory_barrier{ GL_ALL_BARRIER_BITS };
    VEC3 _num_groups{ 0, 0, 0 };
    ComputeObject(const std::string& name);
};