/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 17:38:55 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/17 17:55:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"

namespace GLError {
	GLenum	CheckForError() { return (glGetError()); };
	std::string	GetErrorString(GLenum error_code) {
		switch (error_code) {
			case (GL_NO_ERROR) :
				return ("No GL Error");
			case (GL_INVALID_ENUM) :
				return ("Invalid GLenum");
			case (GL_INVALID_VALUE) :
				return ("Invalid Value");
			case (GL_INVALID_OPERATION) :
				return ("Invalid Operation");
			case (GL_INVALID_FRAMEBUFFER_OPERATION) :
				return ("Invalid Framebuffer Operation");
			case (GL_OUT_OF_MEMORY) :
				return ("Out Of Memory");
		};
		return ("Unknown Error");
	};
}

