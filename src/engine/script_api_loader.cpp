/* Mushroom API Implementation */

/* Registers native API functions with the scripting engine so they may be called by scripts. */

#include "engine/script_engine.hpp"
#include "engine/script_api.hpp"
#include "engine/utils.hpp"
#include "engine/error_return_types.h"

#include <string>

using namespace ScriptEngine;
using std::string;

void ScriptEngine::RegisterAPI (int *error_code, script_engine_t *script_engine)
{
    Utils::TryAndCatchError(
        script_engine->SetMessageCallback(asFUNCTION(scrpt_MessageCallback), 0, asCALL_CDECL) >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not set message log callback.");

    // Register types
    RegisterStdString(script_engine);

    // Register namepspace
    Utils::TryAndCatchError(
        script_engine->SetDefaultNamespace("MUSH") >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not register engine namespace.");

    // Register functions
    bool warningOccured; // API Functions can (but shouldn't) optionally fail to be set.
    string warningMessage = "WARNING::ANGELSCRIPT::Failed to register API function: %s";

    string Print = "void Print(const string &in)";
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(Print.c_str(), asFUNCTION(scrpt_Print), asCALL_CDECL), warningMessage, Print.c_str());

    string GetTime = "float GetTime()";
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(GetTime.c_str(), asFUNCTION(scrpt_GetTime), asCALL_CDECL), warningMessage, GetTime.c_str());

    string GetDeltaTime = "float GetDeltaTime()";
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(GetDeltaTime.c_str(), asFUNCTION(scrpt_GetDeltaTime), asCALL_CDECL), warningMessage, GetDeltaTime.c_str());

    if (warningOccured)
    {
        fprintf(stderr, "WARNING::ANGELSCRIPT::Failed to register all api functions. May cause undefined behaviour or script crashes.\n");
        *error_code = SCRIPT_ENGINE_WARNING;
    }
}