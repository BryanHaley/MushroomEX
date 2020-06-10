/* Mushroom API */

/* Defines functions for scripts to use as part of the engine API. Naming convention: for a function/method/class
 * "Foo" exposed to AngelScript, define a native function/method/class scrpt_Foo and define the AngelScript signature
 * of the function in a string sig_Foo. */

#ifndef SCRIPT_API_HPP
#define SCRIPT_API_HPP

#include "engine/script_engine.hpp"
#include "../../src/angelscript/add_on/scriptstdstring/scriptstdstring.h"
#include "../../src/angelscript/add_on/scriptbuilder/scriptbuilder.h"
#include "../../src/angelscript/add_on/scriptarray/scriptarray.h"

namespace ScriptAPI
{
    // Called by Script Engine Init to register below items with the scripting engine
    void RegisterAPI (int *error_code, ScriptEngine::script_engine_t *script_engine);

    // Print to console wrapper for engine messages (if applicable)
    void scrpt_MessageCallback(const asSMessageInfo *msg, void *param);

    // Mushroom API Namespace
    static const char* sig_Namespace = "MUSH";

    // AsBehaviour interface
    static const char* sig_AsBehaviour_interface     = "AsBehaviour";
    static const char* sig_AsBehaviour_m_Init        = "void Init(int gObjHandle)";
    static const char* sig_AsBehaviour_m_OnActive    = "void OnActive()";
    static const char* sig_AsBehaviour_m_EarlyUpdate = "void EarlyUpdate()";
    static const char* sig_AsBehaviour_m_Update      = "void Update()";
    static const char* sig_AsBehaviour_m_LateUpdate  = "void LateUpdate()";

    // ----- API Functions, Classes, and Methods ------

    // TODO: make this compliant with Doxygen or something and generate an API doc.

    // Global Function : Print
    // Description     : Prints a message to stdout. Typically, the command line.
    // Returns         : void
    // Param           : string in - The message to print to stdout.
    void scrpt_Print (std::string &msg);
    static const char* sig_Print = "void Print(const string &in msg)";
    
    // Global Function : GetTime
    // Description     : Returns the time in seconds at the beginning of the current frame.
    // Returns         : float
    float scrpt_GetTime (void);                                        
    static const char* sig_GetTime = "float GetTime()";

    // Global Function : GetDeltaTime
    // Description     : Returns the time in seconds that elapsed between the beginning of the previous frame and the
    //                   beginning of the current frame.
    // Returns         : float
    float scrpt_GetDeltaTime (void);                                                 
    static const char* sig_GetDeltaTime = "float GetDeltaTime()";

    // Global Function : Translate
    // Description     : Translates the game object of the provided handle by x, y, z in worldspace
    // Returns         : void
    // Param           : int gObjHandle - handle to the game object you want to transform
    // Param           : float x, y, z
    void scrpt_Translate (int gObjHandle, float x, float y, float z);                                        
    static const char* sig_Translate = "void Translate(int gObjHandle, float x, float y, float z)";

    // Global Function : Rotate
    // Description     : Rotates the game object of the provided handle by euler angles yaw (y), pitch (x), and 
    //                 : roll (z) in worldspace
    // Returns         : void
    // Param           : int gObjHandle - handle to the game object you want to transform
    // Param           : float yaw, pitch, roll
    void scrpt_Rotate (int gObjHandle, float yaw, float pitch, float roll);                                        
    static const char* sig_Rotate = "void Rotate(int gObjHandle, float yaw, float pitch, float roll)";
}

#endif