#pragma once

#include "Texture.hpp"

class	Framebuffer : public Texture
{
public :
	static Framebuffer	&create(const std::string &name, VEC2 size, Shader &shader,
				int color_attachements, int depth);
	static void	bind_default();
	void		bind(bool to_bind = true);
	Texture		&attachement(int color_attachement);
	Texture		&depth();
	Shader		&shader();
	void	setup_attachements();
	Texture		&create_attachement(GLenum format, GLenum iformat);
	void	destroy(void *buffer);
	void	resize(const VEC2 &new_size);
private :
	Framebuffer(const std::string &name);
	void		_resize_depth(const VEC2 &);
	void		_resize_attachement(const int &, const VEC2 &);
	void		resize_attachement(const int &, const VEC2 &);
	std::vector<Texture*>	_color_attachements;
	Texture		*_depth;
	Shader		*_shader;
};
