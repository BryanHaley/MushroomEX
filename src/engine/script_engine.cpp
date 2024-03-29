/* Scripting Engine */

/* Handles creating of AngelScript objects and manages interactions between Angelscript objects and native objects.
 * Ideally, this should be a black box and no AngelScript bits should be visible outside of script engine source
 * files. There may come a time where I want to swap out AngelScript for something else, or even support multiple
 * scripting languages. Maybe Duktape JS? Maybe even Mono C#? */

#include "engine/script_engine.hpp"
#include "engine/script_api.hpp"
#include "engine/utils.hpp"
#include "engine/error_return_types.h"

#include <string>

using namespace ScriptEngine;

// Ugly hack to stop segfaulting by losing the reference. TODO: Fix this.
ScriptEngine::script_engine_t *script_engine;

void ScriptEngine::InitEngine (void)
{
    // Create engine
    script_engine = asCreateScriptEngine();
    script_engine->AddRef();

    // Register API
    int error_code;
    ScriptAPI::RegisterAPI(&error_code, script_engine);
    Utils::CheckForWarning(error_code >= NO_ERR, "WARNING::ENGINE::Script Engine API in unknown state.");
}

void ScriptEngine::CreateBehaviourObject (int *error_code, bObj_t *bObj, std::string name, std::string className, std::string script_file)
{
    // Create Behavior Module. Each Behavior runs in its own module.
    CScriptBuilder builder;
    bool warningOccured;

    Utils::TryAndCatchWarning( &warningOccured,
        builder.StartNewModule(script_engine, name.c_str()) >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not create module container for Behaviour Object: %s", name.c_str());
    Utils::TryAndCatchWarning( &warningOccured,
        builder.AddSectionFromFile(script_file.c_str()) >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not bind script to Behaviour Object: %s, Script: %s", name.c_str(), script_file.c_str());
    Utils::TryAndCatchWarning( &warningOccured,
        builder.BuildModule() >= NO_ERR, 
        "ERROR::ANGELSCRIPT::Could not compile script. Check for errors. Script: %s", script_file.c_str());

    if (warningOccured)
    {
        *error_code = SCRIPT_COMPILE_ERROR; // Bound Game Object should delete itself too.
        return;
    }

    // Set module and create context
    bObj->Name    = name;
    bObj->Module  = script_engine->GetModule(name.c_str());
    bObj->Context = script_engine->CreateContext();
    bObj->Context->AddRef();
    bObj->Type    = bObj->Module->GetTypeInfoByDecl(className.c_str());

    // Init the object using its class constructor
    asIScriptFunction *factory = bObj->Type->GetFactoryByDecl((className+" @"+className+"()").c_str());
    bObj->Context->Prepare(factory);
    bObj->Context->Execute();

    // Get the object that was created
    bObj->Object = *(asIScriptObject**)bObj->Context->GetAddressOfReturnValue();
    bObj->Object->AddRef();

    // Bind the object functions to the container
    // TODO: Would be nice to scrap these automatically somehow. Maybe generate the code automatically and #include it.
    bObj->func_Init        = bObj->Type->GetMethodByDecl(ScriptAPI::sig_AsBehaviour_m_Init);
    bObj->func_Init->AddRef();
    bObj->func_OnActive    = bObj->Type->GetMethodByDecl(ScriptAPI::sig_AsBehaviour_m_OnActive);
    bObj->func_OnActive->AddRef();
    bObj->func_EarlyUpdate = bObj->Type->GetMethodByDecl(ScriptAPI::sig_AsBehaviour_m_EarlyUpdate);
    bObj->func_EarlyUpdate->AddRef();
    bObj->func_Update      = bObj->Type->GetMethodByDecl(ScriptAPI::sig_AsBehaviour_m_Update);
    bObj->func_Update->AddRef();
    bObj->func_LateUpdate  = bObj->Type->GetMethodByDecl(ScriptAPI::sig_AsBehaviour_m_LateUpdate);
    bObj->func_LateUpdate->AddRef();

    Utils::CheckForError(bObj->func_Init && bObj->func_Reset && bObj->func_OnActive && bObj->func_EarlyUpdate && bObj->func_Update && bObj->func_LateUpdate,
        "ERROR::ANGELSCRIPT::Could not get methods from Behavior Class: %s", className.c_str());
}

void ScriptEngine::RunBehaviourRoutine (int *error_code, bObj_t *bObj, script_member_func_t *func, bRout_return_t *return_value, int num_args, bRout_arg_t *args)
{
    bObj->Context->Prepare(func);
    bObj->Context->SetObject(bObj->Object);

    // Pass arguments
    if (num_args > 0)
    {
        for (size_t i = 0; i < num_args; i++)
        {
            switch (args[i].type)
            {
                case (ScriptEngine::T_UINT8):
                    bObj->Context->SetArgByte(i, args[i].val_uint8);
                    break;
                case (ScriptEngine::T_UINT16):
                    bObj->Context->SetArgWord(i, args[i].val_uint16);
                    break;
                case (ScriptEngine::T_UINT32):
                    bObj->Context->SetArgDWord(i, args[i].val_uint32);
                    break;
                case (ScriptEngine::T_UINT64):
                    bObj->Context->SetArgQWord(i, args[i].val_uint64);
                    break;
                case (ScriptEngine::T_FLOAT32):
                    bObj->Context->SetArgFloat(i, args[i].val_float32);
                    break;
                case (ScriptEngine::T_FLOAT64):
                    bObj->Context->SetArgDouble(i, args[i].val_uint64);
                    break;
                case (ScriptEngine::T_OBJECT):
                    bObj->Context->SetArgObject(i, args[i].val_object);
                    break;
                default:
                    Utils::CheckForWarning(false, "ERROR::SCRIPT ENGINE::Tried to pass an argument of unknown type to script function.\n");
            }
        }
    }
    
    *error_code = bObj->Context->Execute();

    // Get return
    if (return_value) switch (return_value->type)
    {
        case (ScriptEngine::T_UINT8):
            return_value->val_uint8   = bObj->Context->GetReturnByte();
            break;
        case (ScriptEngine::T_UINT16):
            return_value->val_uint16  = bObj->Context->GetReturnWord();
            break;
        case (ScriptEngine::T_UINT32):
            return_value->val_uint32  = bObj->Context->GetReturnDWord();
            break;
        case (ScriptEngine::T_UINT64):
            return_value->val_uint64  = bObj->Context->GetReturnQWord();
            break;
        case (ScriptEngine::T_FLOAT32):
            return_value->val_float32 = bObj->Context->GetReturnFloat();
            break;
        case (ScriptEngine::T_FLOAT64):
            return_value->val_float64 = bObj->Context->GetReturnDouble();
            break;
        case (ScriptEngine::T_OBJECT):
            return_value->val_object  = bObj->Context->GetReturnObject();
            break;
        default:
            Utils::CheckForWarning(false, "ERROR::SCRIPT ENGINE::Script returned value of unknown type.\n");
    }

    if (*error_code != asEXECUTION_FINISHED)
    {
        if (*error_code == asEXECUTION_EXCEPTION)
        {
            fprintf(stderr, "ERROR::ANGELSCRIPT::An exception with object %s has occurred. Aborting.\n%s\n", bObj->Name.c_str(), bObj->Context->GetExceptionString());
            ScriptEngine::DestroyBehaviourObject(error_code, bObj); // Destroy Behaviour. Bound Game Object should delete itself too.
            *error_code = SCRIPT_RUNTIME_ERROR_FATAL;
        }

        else
        {
            fprintf(stderr, "ERROR::ANGELSCRIPT::An unknown error has occurred with object %s. Continuing.\n", bObj->Name.c_str());
            *error_code = SCRIPT_RUNTIME_ERROR;
        }
    }
}

// Convenience function to call a void script function with no args
void ScriptEngine::RunBehaviourRoutine (int *error_code, bObj_t *bObj, script_member_func_t *func)
{
    ScriptEngine::RunBehaviourRoutine(error_code, bObj, func, NULL, 0, NULL);
}

// Convenience function to call a script function with no args that returns something
void ScriptEngine::RunBehaviourRoutine (int *error_code, bObj_t *bObj, script_member_func_t *func, bRout_return_t *return_value)
{
    ScriptEngine::RunBehaviourRoutine(error_code, bObj, func, return_value, 0, NULL);
}

void ScriptEngine::DestroyBehaviourObject (int *error_code, bObj_t *bObj)
{
    bObj->Object ->Release();
    bObj->Context->Release();

    bObj->Module           = NULL;
    bObj->Context          = NULL;
    bObj->Type             = NULL;
    bObj->Object           = NULL;
    bObj->func_Init        = NULL;
    bObj->func_Reset       = NULL;
    bObj->func_OnActive    = NULL;
    bObj->func_EarlyUpdate = NULL;
    bObj->func_Update      = NULL;
    bObj->func_LateUpdate  = NULL;

    bObj = NULL;
}

void ScriptEngine::DestroyEngine (int *error_code)
{
    script_engine->ShutDownAndRelease();
}