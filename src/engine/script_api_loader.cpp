/* Mushroom API Implementation */

/* Registers native API functions with the scripting engine so they may be called by scripts. */

#include "engine/script_engine.hpp"
#include "engine/script_api.hpp"
#include "engine/utils.hpp"
#include "engine/error_return_types.h"

#include <string>

using namespace ScriptAPI;
using std::string;

void ScriptAPI::RegisterAPI (int *error_code, ScriptEngine::script_engine_t *script_engine)
{
    Utils::TryAndCatchError(
        script_engine->SetMessageCallback(asFUNCTION(scrpt_MessageCallback), 0, asCALL_CDECL) >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not set message log callback.");

    // Register basic types
    RegisterStdString(script_engine);         // string
    RegisterScriptArray(script_engine, true); // arrays

    // Register namepspace
    Utils::TryAndCatchError(
        script_engine->SetDefaultNamespace(sig_Namespace) >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not register engine namespace.");

    // Register Behaviour class interface
    Utils::TryAndCatchError(
        script_engine->RegisterInterface(sig_AsBehaviour_interface) >= NO_ERR,
        "ERROR::ANGELSCRIPT::Could not register %s interface.", sig_AsBehaviour_interface);
    script_engine->RegisterInterfaceMethod(sig_AsBehaviour_interface, sig_AsBehaviour_m_Init);
    script_engine->RegisterInterfaceMethod(sig_AsBehaviour_interface, sig_AsBehaviour_m_OnActive);
    script_engine->RegisterInterfaceMethod(sig_AsBehaviour_interface, sig_AsBehaviour_m_EarlyUpdate);
    script_engine->RegisterInterfaceMethod(sig_AsBehaviour_interface, sig_AsBehaviour_m_Update);
    script_engine->RegisterInterfaceMethod(sig_AsBehaviour_interface, sig_AsBehaviour_m_LateUpdate);

    // Register API functions/methods/classes. TODO: automate this based on what's in script_api.hpp. Maybe generate
    // the code?

    bool warningOccured; // API Functions can (but shouldn't) optionally fail to be set.
    string warningMessage = "WARNING::ANGELSCRIPT::Failed to register API function: %s";

    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(sig_Print, asFUNCTION(scrpt_Print), asCALL_CDECL), warningMessage, sig_Print);
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(sig_GetTime, asFUNCTION(scrpt_GetTime), asCALL_CDECL), warningMessage, sig_GetTime);
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(sig_GetDeltaTime, asFUNCTION(scrpt_GetDeltaTime), asCALL_CDECL), warningMessage, sig_GetDeltaTime);
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(sig_Translate, asFUNCTION(scrpt_Translate), asCALL_CDECL), warningMessage, sig_Translate);
    Utils::TryAndCatchWarning( &warningOccured,
        script_engine->RegisterGlobalFunction(sig_Rotate, asFUNCTION(scrpt_Rotate), asCALL_CDECL), warningMessage, sig_Rotate);

    if (warningOccured)
    {
        fprintf(stderr, "WARNING::ANGELSCRIPT::Failed to register all api functions. May cause undefined behaviour or script crashes.\n");
        *error_code = SCRIPT_ENGINE_WARNING;
    }
}