/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Environment.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/20 18:43:36 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/20 19:05:44 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Object.hpp"

class Cubemap;

class Environment : public Object
{
public:
	static std::shared_ptr<Environment>	create(const std::string &name);
	static std::shared_ptr<Environment>	environment(unsigned index);
	static std::shared_ptr<Environment>	current();
	static void							set_current(std::shared_ptr<Environment>);
	void								unload();
	std::shared_ptr<Cubemap>			diffuse();
	void								set_diffuse(std::shared_ptr<Cubemap>);
	std::shared_ptr<Cubemap>			irradiance();
	void								set_irradiance(std::shared_ptr<Cubemap>);
protected:
	static std::vector<std::shared_ptr<Environment>>	_environments;
	static std::weak_ptr<Environment>	_current;
	std::weak_ptr<Cubemap>				_diffuse;
	std::weak_ptr<Cubemap>				_irradiance;
	Environment(const std::string &name);
	~Environment() = default;
};
