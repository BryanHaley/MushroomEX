/* Scene header
 * Defines the scene struct and methods to load, unload, update, and render it.
 * The scene object in MushroomEX is conceptually similar to Unity's scenes - the active scene
 * holds and manages everything visible and interactable. */

#ifndef SCENE_HPP
#define SCENE_HPP

#include "engine/surface_data.hpp"
#include "engine/gfx.hpp"
#include "game/game_object.hpp"
#include "glm/glm.hpp"
#include <stdint.h>
#include <vector>

/* The use of vectors here is simply for convenience during load time. DO NOT modify the vectors once the game
 * is running. Allocate everything up front, and reuse objects as needed. In the case of overflowing the pool,
 * you may resort to adding more objects to the end, but never ever ever remove or insert anywhere except for
 * at the end. Use the active and alive flags in the Game Object to manage state. In addition to performance
 * concerns, Game Objects rely on an immutable index.  */

namespace Scene {

// Model draw commands are implemented as a singly link list so they can easily be sorted by model index,
// which reduces draw calls as materials/textures don't need to be reloaded.
typedef struct node_draw_model_command_t node_draw_model_command_t;

struct node_draw_model_command_t
{
    int  modelIndex;
    int  transformIndex;

    node_draw_model_command_t *next;
};

typedef struct
{
    // --------------------------------------------------
    // These four vectors form an informal map,
    // where an object at a given index corresponds to
    // the object at that index in the other three vectors.
    // Together, the four objects represent one
    // game object. See game_object.hpp
    // --------------------------------------------------
    std::vector<game_object_t>       GameObjects;
    std::vector<std::string>         GObjNames;
    std::vector<glm::mat4>           Transforms;
    //std::vector<Behaviour>         Behaviours;
    // --------------------------------------------------
    
    uint32_t                         gObjNextFree;      // The index+1 of the last initialized game object. Game 
                                                        //  objects past this index are allocated, but unused.
    std::vector<model_t>             LoadedModels;      // List of loaded models the gobjs can reference by index.
    std::vector<collision_surface_t> CollisionSurfaces; // List of collision surfaces in the scene.
    //std::vector<animation_t>       Animations;        // List of anims the gobjs can reference by index.

    // -- Command queues --------------------------------
    std::vector<node_draw_model_command_t> DrawModelCommands; // Tells gfx engine what to draw and where every frame.
    uint32_t                               drawModelCommandNextFree; // Next free index in DrawModelCommands
    uint32_t                               lastStaticDrawCall; // Number of draw calls corresponding to static geo.
                                                               // Since static geo never changes, it'd be a waste to
                                                               // regenerate the draw calls every frame.
} scene_t;

// Loading
void    LoadFromFile           (int *error_code, scene_t *scene, std::string filename);
void    CreateSurfacesFromFile (int *error_code, std::vector<collision_surface_t> *surfaces, const std::string filename);

// Runtime Operations
void Init   (scene_t* scene); // <-- Call BEFORE loading from file.
void Update (scene_t* scene);
void Draw   (scene_t* scene);
void Unload (scene_t* scene);

// Command Queueing
void QueueModelDraw (scene_t* scene, int modelIndex, int transformIndex);

}
#endif
