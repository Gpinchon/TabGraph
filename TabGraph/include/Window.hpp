/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:35
*/

#pragma once

#include "Property.hpp"

#include <glm/glm.hpp> // for glm::vec2, glm::vec4, s_vec2
#include <string> // for string

namespace Window {
    /**
     * @brief May change regarding media backend
     * May be SDL_Window
    */
    typedef void* Handle;
    
    void SetSize(const glm::ivec2&);
    glm::ivec2 GetSize();
    void Init(const std::string& name, glm::ivec2 resolution);
    void SetFullscreen(const bool& fullscreen);
    void Swap();
    Handle GetHandle();
};