/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/08 18:45:08 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"

class	Framebuffer : public Texture
{
public :
	static Framebuffer	*create(const std::string &name, VEC2 size, Shader &shader,
				int color_attachements, int depth);
	static void	bind_default();
	bool		is_loaded();
	void		load();
	void		bind(bool to_bind = true);
	Texture		&attachement(int color_attachement);
	Texture		&depth();
	Shader		&shader();
	void	setup_attachements();
	Texture		*create_attachement(GLenum format, GLenum iformat);
	void	destroy(void *buffer);
	void	resize(const VEC2 &new_size);
private :
	Framebuffer(const std::string &name);
	void		_resize_depth(const VEC2 &);
	void		_resize_attachement(const int &, const VEC2 &);
	void		resize_attachement(const int &, const VEC2 &);
	std::vector<Texture*>	_color_attachements;
	Texture		*_depth{nullptr};
	Shader		*_shader{nullptr};
};

/*
** Framebuffer Attachements are always loaded by default and cannot be loaded into GPU
*/
class	Attachement : public Texture
{
public :
	static Attachement	*create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi);
	bool		is_loaded();
	void		load();
private :
	Attachement(const std::string &name);
};