#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "engine/error_return_types.h"
#include "engine/gfx.hpp"
#include "engine/level.hpp"

/*
 *  For now just load the triangles from a model with assimp just like mesh_loader.cpp
 *  In the future when I have a level editor set up, I'll want to create a custom OBJ-like file format for static geometry
 *  that will let me encode surfaces into the model data.
 */

void cs_processNode(std::vector<collision_surface_t> surfaces, const std::string directory, aiNode *node, const aiScene *scene);
std::vector<collision_surface_t> cs_processMesh(aiMesh *mesh, const std::string directory, const aiScene *scene);

std::vector<collision_surface_t> create_collision_surfaces_from_file(int *error_code, const std::string directory, const std::string filename, const std::string type)
{
	// Load model into assimp scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(directory + "/" + filename + "." + type, aiProcess_Triangulate);

	std::vector<collision_surface_t> surfaces;

	// error check
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
    	printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
    	*error_code = MODEL_LOADER_ERR;
        return surfaces;
    }

    // recurse through scene tree looking for meshes
    cs_processNode(surfaces, directory, scene->mRootNode, scene);

    *error_code = NO_ERR;
    return surfaces;
}

void cs_processNode(std::vector<collision_surface_t> surfaces, const std::string directory, aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::vector<collision_surface_t> mesh_surfaces = cs_processMesh(mesh, directory, scene);

        for (size_t i = 0; i < mesh_surfaces.size(); i++) surfaces.push_back(mesh_surfaces[i]);
    }
    // continue through tree
    for(size_t i = 0; i < node->mNumChildren; i++)
    {
        cs_processNode(surfaces, directory, node->mChildren[i], scene);
    }

}

std::vector<collision_surface_t> cs_processMesh(aiMesh *mesh, const std::string directory, const aiScene *scene)
{
    std::vector<collision_surface_t> surfaces;

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

        surfaces.push_back(surface);
    }

    return surfaces;
}