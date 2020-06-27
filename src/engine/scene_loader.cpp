/* Scene Loader
 * Handles loading of scenes from a file */

#include "engine/utils.hpp"
#include "game/scene.hpp"
#include "game/game_object.hpp"
#include "engine/gfx.hpp"
#include "engine/script_engine.hpp"
#include "engine/error_return_types.h"

#include "glm/glm.hpp"
#include "yaml-cpp/yaml.h"

#include <string>
#include <iostream>

using namespace Scene;
using std::string;
using std::vector;

void Scene::LoadFromFile (int *error_code, scene_t* scene, string filename)
{
    YAML::Node scene_yaml = YAML::LoadFile(filename);

    // Check to see if yaml is valid. TODO: more robust validation function;
    if (!scene_yaml || !scene_yaml["scene"])
    {
        fprintf(stderr, "ERROR::SCENE::Unable to load scene from file: %s\n", filename.c_str());
        *error_code = FILE_IO_ERR;
        return;
    }

    // Get object filenames from yaml
    vector<string> geoModels, collModels;
    int index = 0;
    scene->gObjNextFree = 0;

    while (scene_yaml["scene"]["objects"][index])
    {
        // TODO: split this up into multiple smaller functions.

        int gObjIndex = scene->gObjNextFree;

        // TODO: Out of memory check

        // Set name and index of Game Object
        scene->GameObjects[gObjIndex].index = gObjIndex;
        scene->GObjNames  [gObjIndex]       = scene_yaml["scene"]["objects"][index]["name"].as<string>();

        bool active = scene_yaml["scene"]["objects"][index]["active"].as<bool>();
        scene->GameObjects[gObjIndex].flags = active ? GOBJ_FLAGS_ACTIVE_AND_ALIVE : GOBJ_FLAG_ACTIVE;

        // TODO: Sort game objects by model so we can avoid unloading and reloading models

        // Generate transform from position and euler rotation
        float position[] = { scene_yaml["scene"]["objects"][index]["pos"][0].as<float>(),
                             scene_yaml["scene"]["objects"][index]["pos"][1].as<float>(), 
                             scene_yaml["scene"]["objects"][index]["pos"][2].as<float>(), };
        float rotation[] = { scene_yaml["scene"]["objects"][index]["rot"][0].as<float>(),
                             scene_yaml["scene"]["objects"][index]["rot"][1].as<float>(), 
                             scene_yaml["scene"]["objects"][index]["rot"][2].as<float>(), };

        scene->Transforms[gObjIndex] = Utils::MatrixFromPosRot(position, rotation);

        // Load Behaviour
        bool isStaticGeo = scene_yaml["scene"]["objects"][index]["behaviour"].as<string>().compare("__STATIC_GEO__") == 0;
        if (!isStaticGeo && scene_yaml["scene"]["objects"][index]["behClass"])
        {
            scene->GameObjects[gObjIndex].flags |= GOBJ_FLAG_HAS_BEHAVIOUR;
            string className = scene_yaml["scene"]["objects"][index]["behClass"].as<string>();

            ScriptEngine::bObj_t bObj;
            ScriptEngine::CreateBehaviourObject(error_code, &bObj, scene->GObjNames[gObjIndex], className, 
                "mnt/" + scene_yaml["scene"]["objects"][index]["behaviour"].as<string>());

            if (*error_code == SCRIPT_COMPILE_ERROR)
            {
                *error_code = SCRIPT_ENGINE_WARNING;

                // Don't continue with creating this game object
                fprintf(stderr, "ERROR::SCENE::Could not create game object \"%s\"\n", scene->GObjNames[gObjIndex].c_str());
                scene->GameObjects[gObjIndex].flags ^= GOBJ_FLAGS_ACTIVE_AND_ALIVE;
                index++;
                continue;
            }

            scene->Behaviours[gObjIndex] = bObj;
        }

        // TODO: manage loading of models so we don't load duplicate models.
        // Load visual model. TODO: null checks
        scene->LoadedModels.push_back(gfx_create_model(error_code, 
            "mnt/" + scene_yaml["scene"]["objects"][index]["model"].as<string>()));
        scene->GameObjects[gObjIndex].modelIndex = scene->LoadedModels.size()-1;

        if (scene_yaml["scene"]["objects"][index]["collType"].as<int>() != GOBJ_NO_COLLIDER)
            scene->GameObjects[gObjIndex].flags |= GOBJ_FLAG_HAS_COLLISION;

        // Load collision model as surfaces. TODO: null checks
        if (scene_yaml["scene"]["objects"][index]["collType"].as<int>() == GOBJ_SURFACE_COLLIDERS)
        {
            scene->GameObjects[gObjIndex].flags |= GOBJ_FLAG_SURFACE;
            scene->GameObjects[gObjIndex].collisionSurfaceStart = scene->CollisionSurfaces.size();
            Scene::CreateSurfacesFromFile(error_code, isStaticGeo, gObjIndex, scene->CollisionSurfaces, 
                "mnt/" + scene_yaml["scene"]["objects"][index]["collFile"].as<string>());
            scene->GameObjects[gObjIndex].collisionSurfaceEnd   = scene->CollisionSurfaces.size();
        }

        // TODO: Collision cylinders

        // Increment index of next free game object
        scene->gObjNextFree++;

        // Check for potential overflow and try to prevent it and further overflows
        if (scene->gObjNextFree == scene->GameObjects.size())
        {
            scene->GameObjects.resize (scene->GameObjects.size()+100);
            scene->GObjNames  .resize (scene->GameObjects.size()+100);
            scene->Transforms .resize (scene->GameObjects.size()+100);
        }

        index++;
    }
}