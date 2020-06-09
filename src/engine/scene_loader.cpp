/* Scene Loader
 * Handles loading of scenes from a file */

#include "engine/utils.hpp"
#include "game/scene.hpp"
#include "engine/gfx.hpp"
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
        fprintf(stderr, "ERROR::LEVEL::Unable to load level from file: %s\n", filename.c_str());
        *error_code = FILE_IO_ERR;
        return;
    }

    // Get object filenames from yaml
    vector<string> geoModels, collModels;
    int index = 0;

    while (scene_yaml["scene"]["objects"][index])
    {
        int gObjIndex = scene->gObjNextFree;

        // TODO: Out of memory check

        // Set name and index of Game Object
        scene->GameObjects[gObjIndex].index = gObjIndex;
        scene->GObjNames  [gObjIndex]       = scene_yaml["scene"]["objects"][index]["name"].as<string>();
        // TODO: Sort game objects by model so we can avoid unloading and reloading models

        // Generate transform from position and euler rotation
        float position[] = { scene_yaml["scene"]["objects"][index]["pos"][0].as<float>(),
                             scene_yaml["scene"]["objects"][index]["pos"][1].as<float>(), 
                             scene_yaml["scene"]["objects"][index]["pos"][2].as<float>(), };
        float rotation[] = { scene_yaml["scene"]["objects"][index]["rot"][0].as<float>(),
                             scene_yaml["scene"]["objects"][index]["rot"][1].as<float>(), 
                             scene_yaml["scene"]["objects"][index]["rot"][2].as<float>(), };

        scene->Transforms[gObjIndex] = Utils::MatrixFromPosRot(position, rotation);


        // TODO: manage loading of models so we don't load duplicate models.
        // Load visual model. TODO: null checks
        scene->LoadedModels.push_back(gfx_create_model(error_code, 
            "mnt/" + scene_yaml["scene"]["objects"][index]["model"].as<string>()));
        scene->GameObjects[gObjIndex].modelIndex = scene->LoadedModels.size()-1;

        // Load collision model as surfaces. TODO: null checks
        if (scene_yaml["scene"]["objects"][index]["collType"].as<int>() == GOBJ_SURFACE_COLLIDERS)
        {
            scene->GameObjects[gObjIndex].collisionSurfaceStart = scene->CollisionSurfaces.size();
            Scene::CreateSurfacesFromFile(error_code, &scene->CollisionSurfaces, 
                "mnt/" + scene_yaml["scene"]["objects"][index]["collFile"].as<string>());
            scene->GameObjects[gObjIndex].collisionSurfaceEnd   = scene->CollisionSurfaces.size();
        }

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