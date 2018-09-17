/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureArray.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 15:59:46 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/17 19:38:33 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TextureArray.hpp"
#include "Engine.hpp"
#include "Errors.hpp"
#include <algorithm>

TextureArray::TextureArray(const std::string &name, VEC2 s, GLenum target, GLenum fi, unsigned capacity) : Texture(name)
{
	_size = s;
	_target = target;
	_internal_format = fi;
	_capacity = capacity;
	_textures.reserve(capacity);
}

TextureArray	*TextureArray::create(const std::string &name, VEC2 s, GLenum target, GLenum fi, unsigned capacity)
{
	auto	texture = new TextureArray(name, s, target, fi, capacity);
	Engine::add(*texture);
	return (texture);
}

int			TextureArray::add(Texture *texture)
{
	auto	search = std::find(_textures.begin(), _textures.end(), texture);
	if (search != _textures.end())
		return (search - _textures.begin());
	if (_textures.size() == _capacity)
		return (-1); //Texture array is full
	_textures.push_back(texture);
	return (_textures.size() - 1);
}

void		TextureArray::load()
{
	if (_loaded)
		return ;
	glGenTextures(1, &_glid);
	glBindTexture(_target, _glid);
	glTexStorage3D(_target, 1, _internal_format, _size.x, _size.y, _capacity);
	for (auto i = 0u; i < _textures.size(); i++)
	{
		auto t = _textures.at(i);
		if (!t->is_loaded())
			t->load();
		//glTextureSubImage3D(t->glid(), 0, 0, 0, i, t->size().x, t->size().y, 1, t->format(), t->data_format(), t->data());
	}
	glTexParameteri(_target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(_target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(_target,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(_target,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glBindTexture(_target, 0);
	_loaded = true;
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
	auto	error = GLError::CheckForError();
	if (error != GL_NO_ERROR)
		throw std::runtime_error(std::string("Error at TextureArray::load ") + name() + " : " + GLError::GetErrorString(error));
#endif //GL_DEBUG
}