/* Scene Loader
 * Handles loading of scenes from a file */

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

namespace Scene {
    void LoadLevelGeometry  (int *error_code, scene_t* scene, vector<string> *p_GeometryFiles);
    void LoadLevelCollision (int *error_code, scene_t* scene, vector<string> *p_CollisionFiles);
}

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

        // Test. TODO: Do this properly. I want to get to bed.
        scene->GameObjects[gObjIndex].index      = gObjIndex;
        scene->GameObjects[gObjIndex].modelIndex = 0;
        scene->GObjNames  [gObjIndex]            = scene_yaml["scene"]["objects"][index]["name"].as<string>();
        scene->Transforms [gObjIndex]            = glm::mat4(1.0f);

        // Get rid of this part. Just load the models as gobjs use them.
        if (scene_yaml["scene"]["objects"][index]["model"]) 
        geoModels.push_back(scene_yaml["scene"]["objects"][index]["model"].as<string>());

        if (scene_yaml["scene"]["objects"][index]["collType"].as<int>() == GOBJ_SURFACE_COLLIDERS)
        collModels.push_back(scene_yaml["scene"]["objects"][index]["collFile"].as<string>());

        index++;
    }

    // Load objects from files
    Scene::LoadLevelGeometry(error_code, scene, &geoModels);
    Scene::LoadLevelCollision(error_code, scene, &collModels);
}

void Scene::LoadLevelGeometry (int *error_code, scene_t* scene, vector<string> *geometryFiles)
{
    for (size_t i = 0; i < geometryFiles->size(); i++)
    {
        if ((*geometryFiles)[i].compare("") != 0)
        {
            // Decompose filename
            string directory = (*geometryFiles)[i].substr(0, (*geometryFiles)[i].find_last_of('/'));
            string filename  = (*geometryFiles)[i].substr((*geometryFiles)[i].find_last_of('/')+1, (*geometryFiles)[i].find_last_of('.')-(*geometryFiles)[i].find_last_of('/')-1);
            string type      = (*geometryFiles)[i].substr((*geometryFiles)[i].find_last_of('.')+1);

            model_t model = gfx_create_model(error_code, "mnt/" + directory, filename, type);

            if (*error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::LEVEL::Could not load geometry from file: %s\n", (*geometryFiles)[i].c_str());
                return;
            }

            scene->LoadedModels.push_back(model);
        }
    }
}

void Scene::LoadLevelCollision (int *error_code, scene_t* scene, vector<string> *collisionFiles)
{
    for (size_t i = 0; i < collisionFiles->size(); i++)
    {
        if ((*collisionFiles)[i].compare("") != 0)
        {
            Scene::CreateSurfacesFromFile(error_code, &scene->CollisionSurfaces, "mnt/" + (*collisionFiles)[i]);
            if (*error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::LEVEL::Could not load collision surfaces from file: %s\n", (*collisionFiles)[i].c_str());
                return;
            }
        }
    }
}