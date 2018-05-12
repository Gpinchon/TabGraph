#pragma once

#include "GLIncludes.hpp"
#include "Renderable.hpp"
#include "AABB.hpp"

struct  CVEC4
{
	unsigned char	x;
	unsigned char	y;
	unsigned char	z;
	unsigned char	w;
};

class	Mesh : public Renderable
{
public:
	static Mesh	*create(const std::string &);
	static Mesh	*get_by_name(const std::string &);
    void		load();
	void		bind();
	void		render();
	void		center();
	std::vector<VEC3>	v;
	std::vector<CVEC4>	vn;
	std::vector<VEC2>	vt;
	VEC2		uvmax;
	VEC2		uvmin;
	GLuint		v_arrayid;
	GLuint		v_bufferid;
	GLuint		vn_bufferid;
	GLuint		vt_bufferid;
private:
	Mesh(const std::string &name);
};
