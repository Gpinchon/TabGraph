/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-16 14:47:28
*/

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

/** @brief This class orchestrates the rendering loop and logics around it
*/
namespace Engine {
    void    Init(void);
    void    Start(void);
    void    Stop(void);
    void    SetInternalQuality(float);
    void    SetSwapInterval(int8_t);

    float   InternalQuality(void);
    int8_t  SwapInterval(void);
    double  DeltaTime(void);
    double  FixedDeltaTime(void);
    const std::string&  ExecutionPath(void);
    const std::string&  ProgramPath(void);
    const std::string   ResourcePath(void);
    std::mutex          &UpdateMutex(void);
};
