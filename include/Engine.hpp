/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-04 14:08:59
*/

#pragma once

#include <string>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif // M_PI
#define UP new_vec3(0, 1, 0)

namespace std
{
    class mutex;
}

/** @brief This class orchestrates the rendering loop and logics around it
*/
namespace Engine {
    /** @brief Initializes the Engine */
    void    Init(void);
    /** @brief Starts Engine loop */
    void    Start(void);
    /** @brief Tells the Engine to stop the loop */
    void    Stop(void);
    /** @brief Sets the Swap Interval
        @arg inerval : See https://wiki.libsdl.org/SDL_GL_SetSwapInterval
    */
    void    SetSwapInterval(int8_t interval);
    /** @brief Returns the current Swap Interval
        @return See https://wiki.libsdl.org/SDL_GL_SetSwapInterval
    */
    int8_t  SwapInterval(void);
    /** @return Timing between the last two Updates */
    double  DeltaTime(void);
    /** @return Timing between the last two Fixed Updates, mainly for event, 0.016 or more */
    double  FixedDeltaTime(void);
    /** @return See http://manpagesfr.free.fr/man/man3/getcwd.3.html */
    const std::string&  ExecutionPath(void);
    /** @return See https://wiki.libsdl.org/SDL_GetBasePath */
    const std::string&  ProgramPath(void);
    /** @return ProgramPath + "/res/" */
    const std::string   ResourcePath(void);
    /** @brief Lock this mutex to keep Engine from performing next update, usefull in multithreaded applications */
    std::mutex          &UpdateMutex(void);
};
