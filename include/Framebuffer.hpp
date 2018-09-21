/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:05:31 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"

class	Framebuffer : public Texture
{
public :
	static std::shared_ptr<Framebuffer>	create(const std::string &name, VEC2 size, int color_attachements, int depth);
	static std::shared_ptr<Framebuffer>	get_by_name(const std::string &name);
	static std::shared_ptr<Framebuffer>	get(unsigned index);
	static void					bind_default();
	bool						is_loaded();
	void						load();
	void						bind(bool to_bind = true);
	std::shared_ptr<Texture>	attachement(unsigned color_attachement);
	std::shared_ptr<Texture>	depth();
	void						setup_attachements();
	std::shared_ptr<Texture>	create_attachement(GLenum format, GLenum iformat);
	//void						destroy(void *buffer);
	void						resize(const VEC2 &new_size);
	void						set_attachement(unsigned color_attachement, std::shared_ptr<Texture>);
private :
	static std::vector<std::shared_ptr<Framebuffer>> _framebuffers;
	Framebuffer(const std::string &name);
	void						_resize_depth(const VEC2 &);
	void						_resize_attachement(const int &, const VEC2 &);
	void						resize_attachement(const int &, const VEC2 &);
	std::vector<std::shared_ptr<Texture>>		_color_attachements;
	std::shared_ptr<Texture>					_depth;
};

/*
** Framebuffer Attachements are always loaded by default and cannot be loaded into GPU
*/
class	Attachement : public Texture
{
public :
	static std::shared_ptr<Attachement>	create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi);
	bool						is_loaded();
	void						load();
	void						unload();
private :
	Attachement(const std::string &name);
	Attachement(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format);
};
