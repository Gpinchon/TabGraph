/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Engine.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 20:02:35 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/22 22:25:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "Object.hpp"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif // M_PI
#define UP new_vec3(0, 1, 0)
//#define DEBUG_MOD

class Material;
class Shader;
class Texture;
class Framebuffer;
class Cubemap;
class Renderable;
class Node;
class Camera;
class Light;

class Engine {
public:
    ~Engine();
    static void init(void);
    static double delta_time(void);
    static double fixed_delta_time(void);
    static void run(void);
    static void stop(void) { _get()._loop = false; };
    static float& internal_quality(void);
    static int8_t& swap_interval(void);
    static void fixed_update(void);
    static void update(void);
    static std::string& execution_path(void);
    static std::string& program_path(void);
    static const std::string& resource_path(void);
    static std::mutex &update_mutex(void);
    static bool loop();

private:
    Engine();
    static Engine& _get();
    static Engine* _instance;
    static void _renderingThread(void);
    void _set_program_path(std::string& argv0);
    void _load_res();
    bool _loop{ false };
    int8_t _swap_interval{ 1 };
    double _delta_time{ 0 };
    std::string _program_path{ "" };
    std::string _exec_path{ "" };
    float _internal_quality{ 1 };
    std::mutex _update_mutex;
};
