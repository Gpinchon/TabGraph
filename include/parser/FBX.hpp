/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FBX.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/11 17:26:44 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/11 18:06:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.hpp"

class	FBX : public Mesh
{
public:
	static void parseBin(const std::string &);
private:
	virtual void abstract() = 0;
};
