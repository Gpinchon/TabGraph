/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vgroup.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:43:25 by gpinchon          #+#    #+#             */
/*   Updated: 2019/04/06 16:10:35 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

struct CVEC4 {
    GLubyte x;
    GLubyte y;
    GLubyte z;
    GLubyte w;
};

class VertexArray;
class Material;

class Vgroup : public Renderable {
public:
    static std::shared_ptr<Vgroup> create(const std::string&);
    static std::shared_ptr<Vgroup> get(unsigned index);
    static std::shared_ptr<Vgroup> get_by_name(const std::string& name);
    std::shared_ptr<Material> material();
    void set_material(std::shared_ptr<Material>);
    void bind();
    void load();
    bool render(RenderMod mod = RenderAll);
    bool render_depth(RenderMod mod = RenderAll);
    void center(VEC3& center);
    VEC2 uvmin;
    VEC2 uvmax;
    std::vector<VEC3> v;
    std::vector<CVEC4> vn;
    std::vector<VEC2> vt;
    std::vector<unsigned> i;

protected:
    static std::vector<std::shared_ptr<Vgroup>> _vgroups;
    std::weak_ptr<Material> _material;
    std::weak_ptr<VertexArray> _vao;
    Vgroup(const std::string&);
};