/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeObject.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/24 16:57:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 18:15:01 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

class	Shader;

class ComputeObject : public Renderable
{
public :
	static std::shared_ptr<ComputeObject>	create(const std::string &name, std::shared_ptr<Shader> computeShader);
	virtual std::shared_ptr<Shader>			shader();
	virtual bool							render(RenderMod mod = RenderAll);
	virtual bool							render_depth(RenderMod mod = RenderAll);
	virtual void							set_shader(std::shared_ptr<Shader>);
	virtual void							load();
protected :
	std::vector<std::shared_ptr<ComputeObject>>	_compute_objects;
	std::weak_ptr<Shader>						_shader;
	ComputeObject(const std::string &name);
};