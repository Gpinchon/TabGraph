/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2017/02/18 21:00:58 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCOPE_H
# define SCOPE_H

# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
# include <ezmem.h>
# include <vml.h>

typedef struct	s_window
{
	SDL_Window		*windows;
	SDL_GLContext	glcontext;
}				t_window;

typedef struct	s_engine
{
	ARRAY		windows;

}				t_engine;

#endif