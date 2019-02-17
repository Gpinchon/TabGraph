/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/17 17:38:55 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 14:52:40 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include "GLIncludes.hpp"
#include <iostream>
#ifdef DEBUG_MOD
#define _debugStream(func, line) std::cerr << func << " at line [" << line << "] : "
#define debugLog(message) _debugStream(__PRETTY_FUNCTION__, __LINE__) << message << std::endl;
#define glCheckError() _glCheckError(__PRETTY_FUNCTION__, __LINE__)
auto _glCheckError(const char *func, const int line)
{
	GLenum errorCode;
	GLenum errorRet = GL_NO_ERROR;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		errorRet |= errorCode;
		std::string error;
		switch (errorCode)
		{
			case GL_INVALID_ENUM:                  error = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "GL_STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "GL_OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		_debugStream(func, line) << error << std::endl;
	}
	return errorRet;
}
#else
#define debugLog(message)
#define glCheckError() _glCheckError()
inline auto _glCheckError()
{
	return (GL_NO_ERROR);
}
#endif


