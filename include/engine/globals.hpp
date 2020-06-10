/* Globals */

/* Global variables for the engine. These should be removed. This is pretty much only for rapid prototyping. */

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "game/scene.hpp"
#include "engine/script_engine.hpp"

extern float g_CurrentFrameTime, g_LastFrameTime, g_DeltaTime;
extern Scene::scene_t *CurrentScene;
extern ScriptEngine::script_engine_t *script_engine;

#endif