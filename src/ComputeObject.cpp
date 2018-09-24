/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeObject.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/24 17:00:20 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 18:15:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ComputeObject.hpp"
#include "Shader.hpp"

ComputeObject::ComputeObject(const std::string &name) : Renderable(name) {}

std::shared_ptr<ComputeObject>	ComputeObject::create(const std::string &name, std::shared_ptr<Shader> computeShader)
{
	auto	obj = std::shared_ptr<ComputeObject>(new ComputeObject(name));
	obj->_shader = computeShader;
	_nodes.push_back(obj);
	return (obj);
}

std::shared_ptr<Shader>			ComputeObject::shader()
{
	return (_shader.lock());
}

void							ComputeObject::set_shader(std::shared_ptr<Shader> ishader)
{
	_shader = ishader;
}

bool							ComputeObject::render(RenderMod /*mod*/)
{
	return (false);
}

bool							ComputeObject::render_depth(RenderMod /*mod*/)
{
	return (false);
}

void							ComputeObject::load()
{

}
