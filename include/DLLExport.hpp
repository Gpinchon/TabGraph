/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DLLExport.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/17 20:26:28 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 21:22:16 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifdef _WIN32

#include <windows.h>

#ifdef USE_HIGH_PERFORMANCE_GPU

extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#endif //USE_HIGH_PERFORMANCE_GPU

#endif //_WIN32