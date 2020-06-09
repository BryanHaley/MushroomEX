/* Scene Loader
 * Functions that handle scene operations during runtime. */

#include "engine/utils.hpp"
#include "game/scene.hpp"

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
    scene->drawModelCommandNextFree = 0;

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
    //scene->Behaviours      .resize (OBJECT_BANK_SIZE);
    //Utils::CheckIfVectorOutOfMemory(&scene->Behaviours, OBJECT_BANK_SIZE);
    scene->DrawModelCommands .resize (OBJECT_BANK_SIZE);
    //Utils::CheckIfVectorOutOfMemory(&scene->DrawModelCommands, OBJECT_BANK_SIZE);
}

void Scene::Update (scene_t* scene)
{
    for (size_t i = 0; i < scene->GameObjects.size(); i++)
    {
        //if (scene->GameObjects[i].flags & GOBJ_FLAG_ACTIVE) 
        //scene->Behaviours[i].update();
    }
}

void Scene::Draw (scene_t* scene)
{
    for (size_t i = 0; i < scene->GameObjects.size(); i++)
    {
        if (scene->GameObjects[i].flags & GOBJ_FLAG_ACTIVE)
        gfx_draw_model(&scene->LoadedModels[scene->GameObjects[i].modelIndex], scene->Transforms[i]);
    }
}

void Scene::Unload (scene_t* scene)
{
    free(scene);
    scene = NULL;
}