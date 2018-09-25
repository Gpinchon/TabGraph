/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeObject.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/24 16:57:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/25 18:27:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

class	Shader;
class	Texture;

class ComputeObject : public Renderable
{
public :
	static std::shared_ptr<ComputeObject>	create(const std::string &name, std::shared_ptr<Shader> computeShader);
	virtual std::shared_ptr<Shader>			shader();
	virtual bool							render(RenderMod mod = RenderAll);
	virtual std::shared_ptr<Texture>		in_texture();
	virtual std::shared_ptr<Texture>		out_texture();
	virtual void							set_in_texture(std::shared_ptr<Texture>);
	virtual void							set_out_texture(std::shared_ptr<Texture>);
	virtual bool							render_depth(RenderMod mod = RenderAll);
	virtual void							set_shader(std::shared_ptr<Shader>);
	virtual void							load();
	virtual void							run();
protected :
	std::vector<std::shared_ptr<ComputeObject>>	_compute_objects;
	std::weak_ptr<Texture>						_in_texture;
	std::weak_ptr<Texture>						_out_texture;
	std::weak_ptr<Shader>						_shader;
	ComputeObject(const std::string &name);
};