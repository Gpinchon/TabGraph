/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:31:19 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:13:12 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void			set_key_callback(SDL_Scancode key, t_kcallback callback)
{
	engine_get()->kcallbacks[key] = callback;
}

void			set_refresh_callback(t_kcallback callback)
{
	engine_get()->rcallback = callback;
}