/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 17:50:25 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 21:13:30 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include <map>

class InputDevice;

typedef void	(*t_callback)(SDL_Event *event);

class Events
{
public:
	static void	add(InputDevice *device, SDL_EventType event_type);
	static int	filter(void *e, SDL_Event *event);
	static int	refresh(void);
	static void	set_refresh_callback(t_callback callback);
private :
	static void	window(SDL_Event *event);
	static Events	&_get();
	static Events	*_instance;
	t_callback		_rcallback{nullptr};
	std::map<Uint32, InputDevice*> _input_devices;
	Events();
};
