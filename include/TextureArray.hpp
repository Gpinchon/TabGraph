/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextureArray.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 15:56:13 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/18 11:57:06 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"
#include <vector>

class TextureArray : public Texture
{
public :
	static TextureArray	*create(const std::string &name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
	virtual void	set(Texture *texture, int index);
	//virtual int		add(Texture *texture);
	virtual void	load();
protected :
	TextureArray(const std::string &name, VEC2 s, GLenum target, GLenum fi, unsigned capacity);
	unsigned				_capacity{0};
	std::vector<Texture *>	_textures;
};
