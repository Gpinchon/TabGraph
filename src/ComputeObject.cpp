/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeObject.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/24 17:00:20 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/25 18:51:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ComputeObject.hpp"
#include "Shader.hpp"

ComputeObject::ComputeObject(const std::string &name) : Renderable(name) {}

std::shared_ptr<ComputeObject>	ComputeObject::create(const std::string &name, std::shared_ptr<Shader> computeShader)
{
	auto	obj = std::shared_ptr<ComputeObject>(new ComputeObject(name));
	obj->_shader = computeShader;
	Renderable::add(obj);
	Node::add(obj);
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

void							ComputeObject::run()
{
	/*auto	shaderPtr = shader();
	auto	inTexturePtr = in_texture();
	auto	outTexturePtr = out_texture();
	if (shaderPtr == nullptr)
		return ;
	shaderPtr->use();
	if (inTexturePtr == outTexturePtr) {
		shaderPtr->bind_texture("in_data")
	}
	shaderPtr->use(false);*/
}

std::shared_ptr<Texture>		ComputeObject::out_texture()
{
	return (_out_texture.lock());
}

void							ComputeObject::set_out_texture(std::shared_ptr<Texture> itexture)
{
	_out_texture = itexture;
}

std::shared_ptr<Texture>		ComputeObject::in_texture()
{
	return (_in_texture.lock());
}

void							ComputeObject::set_in_texture(std::shared_ptr<Texture> itexture)
{
	_in_texture = itexture;
}
