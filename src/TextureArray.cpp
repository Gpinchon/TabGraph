/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureArray.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 15:59:46 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/18 18:06:15 by gpinchon         ###   ########.fr       */
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
	_textures.resize(capacity);
}

TextureArray	*TextureArray::create(const std::string &name, VEC2 s, GLenum target, GLenum fi, unsigned capacity)
{
	auto	t = new TextureArray(name, s, target, fi, capacity);
	glGenTextures(1, &t->_glid);
	glBindTexture(t->_target, t->_glid);
	glTexStorage3D(t->_target, 1, t->_internal_format, t->_size.x, t->_size.y, t->_capacity);
	glBindTexture(t->_target, 0);
	t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	t->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	t->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef GL_DEBUG
	glCheckError();
#endif //GL_DEBUG
	Engine::add(*t);
	return (t);
}

/*int			TextureArray::add(Texture *texture)
{
	auto	search = std::find(_textures.begin(), _textures.end(), texture);
	if (search != _textures.end())
		return (search - _textures.begin());
	if (_textures.size() == _capacity)
		return (-1); //Texture array is full
	_textures.push_back(texture);
	return (_textures.size() - 1);
}*/

void		TextureArray::set(Texture *texture, int index)
{
	_textures.at(index) = texture;
}

void		TextureArray::load()
{
	if (_loaded)
		return ;
	glBindTexture(_target, _glid);
	for (auto i = 0u; i < _textures.size(); i++)
	{
		auto t = _textures.at(i);
		if (nullptr == t || nullptr == t->data())
			continue ;
		if (!t->is_loaded())
			t->load();
		glTexSubImage3D(_target, 0, 0, 0, i, t->size().x, t->size().y, 1, t->format(), t->data_format(), t->data());
	}
	glBindTexture(_target, 0);
	_loaded = true;
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
	glCheckError();
#endif //GL_DEBUG
}