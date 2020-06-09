#ifndef SCRIPT_ENGINE_HPP
#define SCRIPT_ENGINE_HPP

#include "angelscript.h"
#include "../../src/angelscript/add_on/scriptstdstring/scriptstdstring.h"
#include "../../src/angelscript/add_on/scriptbuilder/scriptbuilder.h"

#include <stdint.h>

namespace ScriptEngine
{
    typedef asIScriptEngine   script_engine_t;
    typedef asIScriptModule   script_module_t;
    typedef asIScriptContext  script_context_t;
    typedef asITypeInfo       script_type_t;
    typedef asIScriptObject   script_object_t;
    typedef asIScriptFunction script_global_func_t;
    typedef asIScriptFunction script_member_func_t;

    typedef struct
    {
        std::string           Name;
        script_module_t      *Module;
        script_context_t     *Context;
        script_type_t        *Type;
        script_object_t      *Object;
        script_member_func_t *func_Init;
        script_member_func_t *func_Reset;
        script_member_func_t *func_OnActive;
        script_member_func_t *func_EarlyUpdate;
        script_member_func_t *func_Update;
        script_member_func_t *func_LateUpdate;
    } behaviour_object_container_t;

    typedef behaviour_object_container_t bObj_t;

    void      InitEngine                 (int *error_code);
    void      CreateBehaviourObject      (int *error_code, bObj_t *bObj, std::string name, std::string className, std::string script_file);
    void      RunBehaviourRoutine        (int *error_code, bObj_t *bObj, script_member_func_t *func);
    void      DestroyBehaviourObject     (int *error_code, bObj_t *bObj);
    void      DestroyEngine              (int *error_code);

    //template <typename T>
    //void      RegisterGlobalNativeFunc   (int *error_code, T returnType, void* func, ...);
}

// Ugly hack to stop segfaulting by losing the reference. TODO: Fix this.
extern ScriptEngine::script_engine_t *script_engine;

#endif