/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderable.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/20 19:05:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"
#include <unordered_set>

enum RenderMod
{
	RenderAll, RenderOpaque, RenderTransparent
};

class Material;

class	Renderable : public Node
{
public:
	static std::shared_ptr<Renderable>	renderable(unsigned index);
	static std::shared_ptr<Renderable>	get_by_name(const std::string &);
	virtual bool						render(RenderMod mod = RenderAll) = 0;
	virtual bool						render_depth(RenderMod mod = RenderAll) = 0;
	virtual void						load() = 0;
	virtual void						set_material(std::shared_ptr<Material>);
	virtual std::shared_ptr<Material>	material();
	bool								is_loaded() { return (_is_loaded); };
protected:
	static std::vector<std::shared_ptr<Renderable>>	_renderables;
	std::weak_ptr<Material>				_material;
	bool								_is_loaded;
	Renderable(const std::string &name) : Node(name), _is_loaded(false) {};
};

class RenderableMultiDraw : public Renderable
{
public :
	std::unordered_set<Material*>	materials{0};
protected :
	RenderableMultiDraw(const std::string &name) : Renderable(name) {};
};
