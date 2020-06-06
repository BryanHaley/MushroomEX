// Adapted from information published here: https://learnopengl.com/Model-Loading/Model

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>

#include "SOIL2/SOIL2.h"

#include "engine/error_return_types.h"
#include "engine/gfx.hpp"

void processNode(std::vector<mesh_t> *meshes, const std::string directory, aiNode *node, const aiScene *scene);
mesh_t processMesh(aiMesh *mesh, const std::string directory, const aiScene *scene);

model_t gfx_create_model(int *error, const std::string directory, const std::string filename, const std::string type)
{
	// Load model into assimp scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(directory + "/" + filename + "." + type, aiProcess_Triangulate);

	model_t model;

	// error check
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
    	printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
    	*error = MODEL_LOADER_ERR;
        return model;
    }

    // recurse through scene tree looking for meshes
    processNode(&model.meshes, directory, scene->mRootNode, scene);

    *error = NO_ERR;
    return model;
}

void processNode(std::vector<mesh_t> *meshes, const std::string directory, aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->push_back(processMesh(mesh, directory, scene));
    }
    // continue through tree
    for(size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(meshes, directory, node->mChildren[i], scene);
    }

}

mesh_t processMesh(aiMesh *mesh, const std::string directory, const aiScene *scene)
{
    int num_indices = 0;
    int num_vertices = mesh->mNumVertices;

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        num_indices += mesh->mFaces[i].mNumIndices;
    }
    
    GLfloat* vertices = (GLfloat*) calloc((num_vertices*3)+(num_vertices*3)+(num_vertices*2), sizeof(GLfloat));
    GLuint* indices = (unsigned int*) calloc(num_indices, sizeof(GLuint));

    int last_index = 0;
    for(unsigned int i = 0; i < num_vertices; i++)
    {
        vertices[i*3] = mesh->mVertices[i].x;
        vertices[i*3+1] = mesh->mVertices[i].y;
        vertices[i*3+2] = mesh->mVertices[i].z;
        last_index = i*3+3;
    }

    int new_last_index = 0;
    for(unsigned int i = 0; i < num_vertices; i++)
    {
        vertices[i*3+last_index] = mesh->mNormals[i].x;
        vertices[i*3+1+last_index] = mesh->mNormals[i].y;
        vertices[i*3+2+last_index] = mesh->mNormals[i].z;
        new_last_index = i*3+3+last_index;
    }

    last_index = new_last_index;
    for(unsigned int i = 0; i < num_vertices; i++)
    {
        vertices[i*2+last_index] = mesh->mTextureCoords[0][i].x;
        vertices[i*2+1+last_index] = mesh->mTextureCoords[0][i].y;
    }

    int flCounter = 0;
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];

        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices[flCounter++] = face.mIndices[j];
        }
    }

    // For now just try to load "{materialname}_diffuse.png" for the texture.
    aiString name;
    scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_NAME,name);
    std::string name_str = std::string(name.data);

    std::string diffuse_filename = directory + "/" + name_str + "_diffuse.png";
    std::string normal_filename = directory + "/" + name_str + "_normal.png";
    std::string specular_filename = directory + "/" + name_str + "_specular.png";

    GLuint diffuse_texture = SOIL_load_OGL_texture(diffuse_filename.c_str(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
    GLuint normal_texture = SOIL_load_OGL_texture(normal_filename.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
    GLuint specular_texture = SOIL_load_OGL_texture(specular_filename.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    if (diffuse_texture < 0) fprintf(stderr, "ERROR::UNABLE TO LOAD DIFFUSE TEXTURE %s\n", diffuse_filename.c_str());
    if (normal_texture < 0) fprintf(stderr, "ERROR::UNABLE TO LOAD NORMAL TEXTURE %s\n", normal_filename.c_str());
    if (specular_texture < 0) fprintf(stderr, "ERROR::UNABLE TO LOAD SPECULAR TEXTURE %s\n", specular_filename.c_str());

    mesh_t result_mesh = gfx_create_mesh(num_indices, indices, num_vertices, vertices, diffuse_texture, normal_texture, specular_texture);

    free(vertices);
    free(indices);

    return result_mesh;
}