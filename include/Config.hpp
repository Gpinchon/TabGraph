/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/25 11:36:06 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 22:34:33 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include "GLIncludes.hpp"
#include <string>

class Config {
public:
    static void Load(const std::string&);
    static VEC2& WindowSize();
    static std::string& WindowName();
    static float& Anisotropy();
    static int16_t& MaxTexRes();
    static uint16_t& ShadowRes();
    static uint16_t& Msaa();
    static uint16_t& BloomPass();
    static uint16_t& LightsPerPass();
    static uint16_t& ShadowsPerPass();
    static uint16_t& ReflexionSteps();
    static uint16_t& ReflexionSamples();
    static uint16_t& ReflexionBorderFactor();

private:
    static Config* _get();
    static Config* _instance;
    VEC2 _windowSize { 1280, 720 };
    std::string _windowName { "" };
    float _anisotropy { 16.f };
    int16_t _maxTexRes { 0 };
    uint16_t _shadowRes { 2048 };
    uint16_t _msaa { 0 };
    uint16_t _bloomPass { 1 };
    uint16_t _lightsPerPass { 32 };
    uint16_t _shadowsPerPass { 16 };
    uint16_t _ReflexionSteps { 10 };
    uint16_t _reflexionSamples { 9 };
    uint16_t _reflexionBorderFactor { 10 };
    Config() = default;
    ~Config() = default;
};
