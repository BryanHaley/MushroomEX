/* Mushroom API Implementation */

/* Implementations of API functions for scripts to use. */

#include "engine/script_engine.hpp"
#include "engine/script_api.hpp"
#include "engine/globals.hpp"
#include "engine/error_return_types.h"

#include <string>

using namespace ScriptEngine;
using std::string;

void ScriptEngine::scrpt_MessageCallback(const asSMessageInfo *msg, void *param)
{
    fprintf(stderr, "ERROR::ANGELSCRIPT MESSAGE::%s\n", msg->message);
}

void ScriptEngine::scrpt_Print (string &msg)
{
    printf("%s", msg.c_str());
}

float ScriptEngine::scrpt_GetTime (void)
{
    return g_CurrentFrameTime;
}

float ScriptEngine::scrpt_GetDeltaTime (void)
{
    return g_DeltaTime;
}