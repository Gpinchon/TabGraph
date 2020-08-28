/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:14:37
*/

#pragma once

#include <memory>
#include <mutex> // for mutex
#include <string> // for string
#include <sys/types.h> // for int8_t
#include <filesystem>

#ifndef UP
//#define UP glm::vec3(0, 1, 0);
#endif

/** @brief This class orchestrates the rendering loop and logics around it
*/
namespace Engine {
/** @brief Initializes the Engine */
void Init(void);
/** @brief Starts Engine loop */
void Start(void);
/** @brief Tells the Engine to stop the loop */
void Stop(void);
/** @brief Sets the Swap Interval
        @arg inerval : See https://wiki.libsdl.org/SDL_GL_SetSwapInterval
    */
void SetSwapInterval(int8_t interval);
/** @brief Returns the current Swap Interval
        @return See https://wiki.libsdl.org/SDL_GL_SetSwapInterval
    */
int8_t SwapInterval(void);
/** @return Timing between the last two Updates */
double DeltaTime(void);
/** @return Timing between the last two Fixed Updates, mainly for event, 0.016 or more */
double FixedDeltaTime(void);
/** @return See http://manpagesfr.free.fr/man/man3/getcwd.3.html */
const std::filesystem::path ExecutionPath(void);
/** @return See https://wiki.libsdl.org/SDL_GetBasePath */
const std::filesystem::path ProgramPath(void);
/** @return ProgramPath + "/res/" */
const std::filesystem::path ResourcePath(void);
};
