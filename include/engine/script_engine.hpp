#ifndef SCRIPT_ENGINE_HPP
#define SCRIPT_ENGINE_HPP

#include "angelscript.h"

#include <stdint.h>
#include <string>

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

    typedef enum 
    {
        T_VOID,
        T_UINT8,
        T_UINT16,
        T_UINT32,
        T_UINT64,
        T_FLOAT32,
        T_FLOAT64,
        T_OBJECT
    } behaviour_routine_type_t;

    typedef struct
    {
        behaviour_routine_type_t type;
        union
        {
            uint8_t  val_uint8;
            uint16_t val_uint16;
            uint32_t val_uint32;
            uint64_t val_uint64;
            float    val_float32;
            double   val_float64;
            void*    val_object;
        };
    } behaviour_routine_value_t;

    #define bRout_type_t behaviour_routine_type_t

    #define behaiour_routine_return_value_t behaviour_routine_value_t
    #define bRout_arg_t  behaviour_routine_value_t
    #define bRout_return_t behaviour_routine_value_t

    #define T_CHAR   T_UINT8
    #define T_STRING T_OBJECT
    #define T_FLOAT  T_FLOAT32
    #define T_DOUBLE T_FLOAT64
    #define T_SHORT  T_UINT16
    #define T_INT    T_UINT32
    #define T_LONG   T_UINT64

    #define val_char   val_uint8
    #define val_string val_object
    #define val_float  val_float32
    #define val_double val_float64
    #define val_short  val_uint16
    #define val_int    val_uint32
    #define val_long   val_uint64

    typedef behaviour_object_container_t bObj_t;

    void InitEngine             (void);
    void CreateBehaviourObject  (int *error_code, bObj_t *bObj, std::string name, std::string className, std::string script_file);
    void RunBehaviourRoutine    (int *error_code, bObj_t *bObj, script_member_func_t *func);
    void RunBehaviourRoutine    (int *error_code, bObj_t *bObj, script_member_func_t *func, bRout_return_t *return_value);
    void RunBehaviourRoutine    (int *error_code, bObj_t *bObj, script_member_func_t *func, bRout_return_t *return_value, int num_args, bRout_arg_t *args);
    void DestroyBehaviourObject (int *error_code, bObj_t *bObj);
    void DestroyEngine          (int *error_code);

    //template <typename T>
    //void      RegisterGlobalNativeFunc   (int *error_code, T returnType, void* func, ...);

    // Ugly hack to stop segfaulting by losing the reference. TODO: Fix this.
    
}

namespace ScriptAPI
{
    // Called by Script Engine Init to register native functions/classes/methods with the scripting engine
    void RegisterAPI (int *error_code, ScriptEngine::script_engine_t *script_engine);
}

#endif