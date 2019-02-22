/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputDevice.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 20:13:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 20:16:23 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"

class InputDevice {
public:
    virtual void process_event(SDL_Event*) = 0;

protected:
    InputDevice() = default;
};