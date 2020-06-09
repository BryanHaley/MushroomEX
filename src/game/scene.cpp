/* Scene Loader
 * Functions that handle scene operations during runtime. */

#include "engine/utils.hpp"
#include "engine/script_engine.hpp"
#include "game/scene.hpp"
#include "engine/error_return_types.h"

#include <stdlib.h>

// TODO: Try to choose this somewhat intelligently. Maybe some multiple of how many objects will be loaded from the
// file?
#define OBJECT_BANK_SIZE 512

using namespace Scene;
using std::string;
using std::vector;

void Scene::Init (scene_t* scene)
{
    scene->gObjNextFree = 0;

    // Reserve space for vectors where object reuse is infrequent
    scene->LoadedModels      .reserve(OBJECT_BANK_SIZE);
    scene->CollisionSurfaces .reserve(OBJECT_BANK_SIZE);
    //scene->Animations      .reserve(OBJECT_BANK_SIZE);

    // Allocate objects for vectors where frequent object reuse IS expected. 
    // Make sure to check for out-of-memory errors.
    scene->GameObjects       .resize (OBJECT_BANK_SIZE);
    //Utils::CheckIfVectorOutOfMemory(&scene->GameObjects, OBJECT_BANK_SIZE);
    scene->GObjNames         .resize (OBJECT_BANK_SIZE);
    //Utils::CheckIfVectorOutOfMemory(&scene->GObjNames, OBJECT_BANK_SIZE);
    scene->Transforms        .resize (OBJECT_BANK_SIZE);
    //Utils::CheckIfVectorOutOfMemory(&scene->Transforms, OBJECT_BANK_SIZE);
    scene->Behaviours        .resize (OBJECT_BANK_SIZE);
    //Utils::CheckIfVectorOutOfMemory(&scene->Behaviours, OBJECT_BANK_SIZE);

    int unused; // InitEngine prints out its non-fatal errors already
    ScriptEngine::InitEngine(&unused);
}

void Scene::Start (scene_t* scene)
{
    for (size_t i = 0; i < scene->gObjNextFree; i++)
    {
        if (scene->GameObjects[i].flags & GOBJ_FLAG_ALIVE)
        {
            int error_code;
            ScriptEngine::RunBehaviourRoutine(&error_code, &scene->Behaviours[i], scene->Behaviours[i].func_Init);

            if (error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::GAME OBJECT::%s behaviour %s malfunctioned. Deleting Game Object.\n", scene->GObjNames[i].c_str(), scene->Behaviours[i].Name.c_str());
                scene->GameObjects[i].flags ^= GOBJ_FLAGS_ACTIVE_AND_ALIVE;
            }
        }

        if (scene->GameObjects[i].flags & GOBJ_FLAG_ACTIVE)
        {
            int error_code;
            ScriptEngine::RunBehaviourRoutine(&error_code, &scene->Behaviours[i], scene->Behaviours[i].func_OnActive);

            if (error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::GAME OBJECT::%s behaviour %s malfunctioned. Deleting Game Object.\n", scene->GObjNames[i].c_str(), scene->Behaviours[i].Name.c_str());
                scene->GameObjects[i].flags ^= GOBJ_FLAGS_ACTIVE_AND_ALIVE;
            }
        }
    }
}

void Scene::Update (scene_t* scene)
{
    // Early Update
    for (size_t i = 0; i < scene->gObjNextFree; i++)
    {
        if (scene->GameObjects[i].flags & GOBJ_FLAGS_ACTIVE_AND_ALIVE)
        {
            int error_code;
            ScriptEngine::RunBehaviourRoutine(&error_code, &scene->Behaviours[i], scene->Behaviours[i].func_EarlyUpdate);

            if (error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::GAME OBJECT::%s behaviour %s malfunctioned. Deleting Game Object.\n", scene->GObjNames[i].c_str(), scene->Behaviours[i].Name.c_str());
                scene->GameObjects[i].flags ^= GOBJ_FLAGS_ACTIVE_AND_ALIVE;
            }
        }
    }

    // Update
    for (size_t i = 0; i < scene->gObjNextFree; i++)
    {
        if (scene->GameObjects[i].flags & GOBJ_FLAGS_ACTIVE_AND_ALIVE)
        {
            int error_code;
            ScriptEngine::RunBehaviourRoutine(&error_code, &scene->Behaviours[i], scene->Behaviours[i].func_Update);

            if (error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::GAME OBJECT::%s behaviour %s malfunctioned. Deleting Game Object.\n", scene->GObjNames[i].c_str(), scene->Behaviours[i].Name.c_str());
                scene->GameObjects[i].flags ^= GOBJ_FLAGS_ACTIVE_AND_ALIVE;
            }
        }
    }

    // Late Update
    for (size_t i = 0; i < scene->gObjNextFree; i++)
    {
        if (scene->GameObjects[i].flags & GOBJ_FLAGS_ACTIVE_AND_ALIVE)
        {
            int error_code;
            ScriptEngine::RunBehaviourRoutine(&error_code, &scene->Behaviours[i], scene->Behaviours[i].func_LateUpdate);

            if (error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::GAME OBJECT::%s behaviour %s malfunctioned. Deleting Game Object.\n", scene->GObjNames[i].c_str(), scene->Behaviours[i].Name.c_str());
                scene->GameObjects[i].flags ^= GOBJ_FLAGS_ACTIVE_AND_ALIVE;
            }
        }
    }
}

void Scene::Draw (scene_t* scene)
{
    for (size_t i = 0; i < scene->gObjNextFree; i++)
    {
        if (scene->GameObjects[i].flags & GOBJ_FLAGS_ACTIVE_AND_ALIVE)
        gfx_draw_model(&scene->LoadedModels[scene->GameObjects[i].modelIndex], scene->Transforms[i]);
    }
}

void Scene::Unload (scene_t* scene)
{
    scene->gObjNextFree = 0;

    scene->LoadedModels      .clear();
    scene->CollisionSurfaces .clear();
    //scene->Animations      .clear();
    scene->GameObjects       .clear();
    scene->GObjNames         .clear();
    scene->Transforms        .clear();
    scene->Behaviours        .clear();

    int unused;
    ScriptEngine::DestroyEngine(&unused);
}