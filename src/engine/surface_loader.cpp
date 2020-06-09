#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "engine/error_return_types.h"
#include "engine/gfx.hpp"
#include "engine/surface_data.hpp"
#include "game/scene.hpp"

using namespace Scene;
using std::string;
using std::vector;

namespace Scene {
void ProcessNode(vector<collision_surface_t> *surfaces, aiNode *node, const aiScene *scene);
void ProcessMesh(vector<collision_surface_t> *surfaces, aiMesh *mesh, const aiScene *scene);
}

/* For now just load the triangles from a model with assimp just like mesh_loader.cpp In the future when I
 * have a level editor set up, I'll want to create a custom OBJ-like file format for static geometry that will
 * let me encode surface data along with the triangles of the collision surfaces into a single file. */

void Scene::CreateSurfacesFromFile(int *error_code, vector<collision_surface_t> *surfaces, const string filename)
{
	// Load model into assimp scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate);

	// error check
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
    	printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
    	*error_code = MODEL_LOADER_ERR;
        return;
    }

    // recurse through scene tree looking for meshes
    ProcessNode(surfaces, scene->mRootNode, scene);
}

void Scene::ProcessNode(vector<collision_surface_t> *surfaces, aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(surfaces, mesh, scene);
    }
    // continue through tree
    for(size_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(surfaces, node->mChildren[i], scene);
    }

}

void Scene::ProcessMesh(vector<collision_surface_t> *surfaces, aiMesh *mesh, const aiScene *scene)
{
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];
        collision_surface_t surface;

        // For now, every collision triangle is the default surface type
        surface.flags = SURFACE_DEFAULT;

        // Meshes should be guarenteed to be triangulated thanks to assimp post-process
        assert(face.mNumIndices == 3);

        aiVector3D mesh_v0 = mesh->mVertices[face.mIndices[0]];
        surface.v0 = glm::vec3(mesh_v0.x, mesh_v0.y, mesh_v0.z);
        aiVector3D mesh_v1 = mesh->mVertices[face.mIndices[1]];
        surface.v1 = glm::vec3(mesh_v1.x, mesh_v1.y, mesh_v1.z);
        aiVector3D mesh_v2 = mesh->mVertices[face.mIndices[2]];
        surface.v2 = glm::vec3(mesh_v2.x, mesh_v2.y, mesh_v2.z);

        surfaces->push_back(surface);
    }
}
