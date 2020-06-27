/* Scene header
 * Defines the scene struct and methods to load, unload, update, and render it.
 * The scene object in MushroomEX is conceptually similar to Unity's scenes - the active scene
 * holds and manages everything visible and interactable. */

#ifndef SCENE_HPP
#define SCENE_HPP

#include "engine/surface_data.hpp"
#include "engine/gfx.hpp"
#include "game/game_object.hpp"
#include "engine/script_engine.hpp"

#include "glm/glm.hpp"

#include <stdint.h>
#include <vector>

/* The use of vectors here is simply for convenience during load time. DO NOT modify the vectors once the game
 * is running. Allocate everything up front, and reuse objects as needed. In the case of overflowing the pool,
 * you may resort to adding more objects to the end, but never ever ever remove or insert anywhere except for
 * at the end. Use the active and alive flags in the Game Object to manage state. In addition to performance
 * concerns, Game Objects rely on an immutable index.  */

namespace Scene 
{
    typedef struct
    {
        //-------------------------------------------------------------------------------------------------------------|
        // These four vectors form an informal map, where an object at a given index corresponds to the object at that |
        // index in the other three vectors.Together, the four objects represent one game object. See game_object.hpp  |
        //-------------------------------------------------------------------------------------------------------------|
        std::vector<game_object_t>         GameObjects;
        std::vector<std::string>           GObjNames;
        std::vector<glm::mat4>             Transforms;
        std::vector<ScriptEngine::bObj_t>  Behaviours;
        //--------------------------------------------------------------------------------------------------------------
        
        //ScriptEngine::script_engine_t  *script_engine;    // Handles behaviours
        uint32_t                         gObjNextFree;      // The index+1 of the last initialized game object. Game 
                                                            //  objects past this index are allocated, but unused.
        std::vector<model_t>             LoadedModels;      // List of loaded models the gobjs can reference by index.
        std::vector<collision_surface_t> CollisionSurfaces; // List of collision surfaces in the scene.
        //std::vector<animation_t>       Animations;        // List of anims the gobjs can reference by index.
    } scene_t;

    // Loading
    void    LoadFromFile           (int *error_code, scene_t *scene, std::string filename);
    void    CreateSurfacesFromFile (int *error_code, bool isStatic, int gObjIndex, 
                                    std::vector<collision_surface_t> &surfaces, const std::string filename);

    // Runtime Operations
    void Init   (scene_t* scene); // <-- Call BEFORE loading from file.
    void Start  (scene_t* scene); // <-- Call AFTER loading from file.
    void Update (scene_t* scene);
    void Draw   (scene_t* scene);
    void Unload (scene_t* scene);

}

#endif
