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
        // etc
    }
}

void Scene::Draw (scene_t* scene)
{
    // Traverse model commands and send them to gfx engine
    node_draw_model_command_t *current = &scene->DrawModelCommands[0];

    while (current != NULL)
    {
        gfx_draw_model(&scene->LoadedModels[current->modelIndex], scene->Transforms[current->transformIndex]);
        current = current->next;
    }

    // Clear the command queue
    scene->DrawModelCommands[0].next = NULL;
    scene->drawModelCommandNextFree = 0;
}

void Scene::Unload (scene_t* scene)
{
    free(scene);
    scene = NULL;
}

void Scene::QueueModelDraw (scene_t* scene, int modelIndex, int transformIndex)
{
    int index = scene->drawModelCommandNextFree;
    // Buffer overflow! Generate more objects to (hopefully) avoid additional overflows.
    if (index == scene->DrawModelCommands.size())
    {
        int newSize = scene->DrawModelCommands.size()+100;
        scene->DrawModelCommands.resize(newSize);

        // Check if we ran out of memory
        //Utils::CheckIfVectorOutOfMemory(&scene->DrawModelCommands, newSize);
    }

    scene->DrawModelCommands[index].modelIndex     = modelIndex;
    scene->DrawModelCommands[index].transformIndex = transformIndex;
    scene->drawModelCommandNextFree++;

    // If we're the new head of the list
    if (index == 0)
    {
        scene->DrawModelCommands[index].next = NULL;
        return;
    }

    // Otherwise, try to find the first draw model command with the same model index and insert ourselves after it. If
    // we don't find a node with the same model index, just insert ourselves at the end of the list.
    node_draw_model_command_t *current = &scene->DrawModelCommands[0];
    node_draw_model_command_t *next    = current->next;

    while (next != NULL)
    {
        // We found a node with the same model index, proceed with insertion
        if (current->modelIndex == scene->DrawModelCommands[index].modelIndex) break;

        // Proceed to next node
        current = next;
        next = current->next;
    }

    // insert the draw model command between the current and next node. If next is null, it's the end of the list.
    scene->DrawModelCommands[index].next = next;
    current->next = &scene->DrawModelCommands[index];
}