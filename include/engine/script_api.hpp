/* Mushroom API */

/* Defines functions for scripts to use as part of the engine API. */

#ifndef SCRIPT_API_HPP
#define SCRIPT_API_HPP

#include "engine/script_engine.hpp"

namespace ScriptEngine
{
    // Called by Script Engine Init to register below functions with the scripting engine
    void RegisterAPI (int *error_code, script_engine_t *script_engine);
    // Print to console wrapper for engine messages (if applicable)
    void scrpt_MessageCallback(const asSMessageInfo *msg, void *param);

    // All functions are to be registered on the scripting side sans the "scrpt_" prefix.
    void  scrpt_Print        (std::string &msg); // Print to the console.
    float scrpt_GetTime      (void);                   // Get current frame time in seconds.
    float scrpt_GetDeltaTime (void);                   // Get time since last frame in seconds.
}

#endif