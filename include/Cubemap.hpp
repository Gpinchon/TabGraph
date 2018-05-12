#pragma once

#include "Texture.hpp"

class Cubemap : public Texture
{
public:
	static Cubemap	*create(const std::string &);
	static Cubemap	*parse(const std::string &, const std::string &);
	Texture			*sides[6];
private:
	Cubemap(const std::string &);
};