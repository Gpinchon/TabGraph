/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderable.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/25 18:47:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

enum RenderMod {
    RenderAll,
    RenderOpaque,
    RenderTransparent
};

class Renderable : public Node {
public:
    static std::shared_ptr<Renderable> get(unsigned index);
    static std::shared_ptr<Renderable> get_by_name(const std::string&);
    virtual bool render(RenderMod mod = RenderAll) = 0;
    virtual bool render_depth(RenderMod mod = RenderAll) = 0;
    virtual void load() = 0;
    bool is_loaded();

protected:
    bool _is_loaded { false };
    static void add(std::shared_ptr<Renderable>);
    Renderable(const std::string& name);

private:
    static std::vector<std::shared_ptr<Renderable>> _renderables;
};
