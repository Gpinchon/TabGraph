/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-08 13:59:15
*/

#pragma once

#include "Object.hpp"
#include "Renderable.hpp" // for RenderAll, RenderMod, Renderable
#include "glm/glm.hpp" // for glm::vec2, s_vec2, s_vec3, glm::vec3
#include <GL/glew.h> // for GLubyte
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Material; // lines 20-20
class VertexArray; // lines 19-19

typedef glm::vec<4, uint8_t, glm::defaultp> CVEC4;

static inline CVEC4 VecToCVec4(glm::vec4 v) {
    v = ((v + 1.f) * 0.5f) * 255.f;
    return CVEC4(v.x, v.y, v.z, v.w);
}

static inline CVEC4 VecToCVec4(glm::vec3 v) {
    v = ((v + 1.f) * 0.5f) * 255.f;
    return CVEC4(v.x, v.y, v.z, 255);
}

class Vgroup : public Object
{
public:
    static std::shared_ptr<Vgroup> Create(const std::string & = "");
    static std::shared_ptr<Vgroup> Get(unsigned index);
    static std::shared_ptr<Vgroup> GetByName(const std::string &name);
    static std::shared_ptr<Vgroup> GetById(int64_t);
    uint32_t MaterialIndex();
    void SetMaterialIndex(uint32_t);
    void Load();
    bool Draw();
    bool IsLoaded() const;
    glm::vec2 uvmin;
    glm::vec2 uvmax;
    std::vector<glm::vec3> v;
    std::vector<CVEC4> vn;
    std::vector<glm::vec2> vt;
    std::vector<unsigned> i;
    BoundingElement *boundingElement;

protected:
    bool _isLoaded{false};
    static std::vector<std::shared_ptr<Vgroup>> _vgroups;
    std::weak_ptr<VertexArray> _vao;
    uint32_t _materialIndex{0};
    Vgroup(const std::string &);
};