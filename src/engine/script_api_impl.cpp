/* Mushroom API Implementation */

/* Implementations of API functions for scripts to use. */

#include "engine/script_engine.hpp"
#include "engine/script_api.hpp"
#include "engine/globals.hpp"
#include "game/scene.hpp"
#include "engine/error_return_types.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/euler_angles.hpp"

#include <string>

using namespace ScriptAPI;
using std::string;

void ScriptAPI::scrpt_MessageCallback(const asSMessageInfo *msg, void *param)
{
    fprintf(stderr, "ERROR::ANGELSCRIPT MESSAGE::%s\n", msg->message);
}

void ScriptAPI::scrpt_Print (string &msg)
{
    printf("%s", msg.c_str());
}

float ScriptAPI::scrpt_GetTime (void)
{
    return g_CurrentFrameTime;
}

float ScriptAPI::scrpt_GetDeltaTime (void)
{
    return g_DeltaTime;
}

void ScriptAPI::scrpt_Translate (int gObjHandle, float x, float y, float z)
{
    glm::mat4 *transform = &CurrentScene->Transforms[gObjHandle];
    glm::vec3 translation(x, y, z);

    *transform = glm::translate(*transform, translation);
}

void ScriptAPI::scrpt_Rotate (int gObjHandle, float yaw, float pitch, float roll)
{
    glm::mat4 *transform = &CurrentScene->Transforms[gObjHandle];
    glm::mat4 rotation = glm::eulerAngleYXZ(yaw, pitch, roll);

    *transform = *transform * rotation;
}