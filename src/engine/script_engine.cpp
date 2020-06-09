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

void ScriptEngine::InitEngine (int *error_code)
{
    // Create engine
    script_engine = asCreateScriptEngine();
    script_engine->AddRef();

    // Register API
    ScriptEngine::RegisterAPI(error_code, script_engine);
    Utils::CheckForWarning(*error_code == NO_ERR, "WARNING::ENGINE::Script Engine API in unknown state.");
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
    bObj->func_Init        = bObj->Type->GetMethodByDecl("void Init()");
    bObj->func_Init->AddRef();
    bObj->func_Reset       = bObj->Type->GetMethodByDecl("void Reset()");
    bObj->func_Reset->AddRef();
    bObj->func_OnActive    = bObj->Type->GetMethodByDecl("void OnActive()");
    bObj->func_OnActive->AddRef();
    bObj->func_EarlyUpdate = bObj->Type->GetMethodByDecl("void EarlyUpdate()");
    bObj->func_EarlyUpdate->AddRef();
    bObj->func_Update      = bObj->Type->GetMethodByDecl("void Update()");
    bObj->func_Update->AddRef();
    bObj->func_LateUpdate  = bObj->Type->GetMethodByDecl("void LateUpdate()");
    bObj->func_LateUpdate->AddRef();

    Utils::CheckForError(bObj->func_Init && bObj->func_Reset && bObj->func_OnActive && bObj->func_EarlyUpdate && bObj->func_Update && bObj->func_LateUpdate,
        "ERROR::ANGELSCRIPT::Could not get methods from Behavior Class: %s", className.c_str());
}

void ScriptEngine::RunBehaviourRoutine (int *error_code, bObj_t *bObj, script_member_func_t *func)
{
    bObj->Context->Prepare(func);
    bObj->Context->SetObject(bObj->Object);
    
    *error_code = bObj->Context->Execute();

    if (*error_code != asEXECUTION_FINISHED)
    {
        if (*error_code == asEXECUTION_EXCEPTION)
        {
            fprintf(stderr, "ERROR::ANGELSCRIPT::An exception with object %s has occurred. Aborting.\n%s\n", bObj->Name.c_str(), bObj->Context->GetExceptionString());
            ScriptEngine::DestroyBehaviourObject(error_code, bObj); // Destroy Behaviour. Bound Game Object should delete itself too.
            *error_code = SCRIPT_RUNTIME_ERROR_FATAL;
            return;
        }

        else
        {
            fprintf(stderr, "ERROR::ANGELSCRIPT::An unknown error has occurred with object %s. Continuing.\n", bObj->Name.c_str());
            *error_code = SCRIPT_RUNTIME_ERROR;
            return;
        }
    }
}

void ScriptEngine::DestroyBehaviourObject (int *error_code, bObj_t *bObj)
{
    bObj->Context->Release();
    bObj->Object ->Release();

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