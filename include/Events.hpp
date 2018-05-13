/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 17:50:25 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/13 17:50:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"

typedef void	(*t_kcallback)(SDL_Event *event);

class Events
{
public:
	static int	filter(void *e, SDL_Event *event);
	static int	refresh(void);
	static void	window(SDL_Event *event);
	static void	keyboard(SDL_Event *event);
	static void	check_events();
	static void	set_key_callback(SDL_Scancode keycode, t_kcallback callback);
	static void	set_refresh_callback(t_kcallback callback);
private :
	static Events	&_get();
	t_kcallback	_kcallbacks[285];
	t_kcallback	_rcallback;
	Events();
};