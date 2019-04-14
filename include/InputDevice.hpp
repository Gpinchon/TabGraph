/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputDevice.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 20:13:51 by gpinchon          #+#    #+#             */
/*   Updated: 2019/04/13 20:25:11 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//#include "GLIncludes.hpp"
#include <SDL2/SDL.h>

class InputDevice {
public:
    virtual void process_event(SDL_Event*) = 0;

protected:
    InputDevice() = default;
};