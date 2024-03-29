// Adapted from information published here: https://learnopengl.com/Model-Loading/Model

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <stdlib.h>

#include "SOIL2/SOIL2.h"

#include "engine/error_return_types.h"
#include "engine/gfx.hpp"

using std::string;

void processNode(std::vector<mesh_t> *meshes, const string filename, aiNode *node, const aiScene *scene);
mesh_t processMesh(aiMesh *mesh, const string filename, const aiScene *scene);

model_t gfx_create_model(int *error_code, const string filename)
{
	// Load model into assimp scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate);

	model_t model;

	// error check
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
    	printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
    	*error_code = MODEL_LOADER_ERR;
        return model;
    }

    // recurse through scene tree looking for meshes
    processNode(&model.meshes, filename, scene->mRootNode, scene);

    *error_code = NO_ERR;
    return model;
}

void processNode(std::vector<mesh_t> *meshes, const string filename, aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->push_back(processMesh(mesh, filename, scene));
    }
    // continue through tree
    for(size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(meshes, filename, node->mChildren[i], scene);
    }

}

mesh_t processMesh(aiMesh *mesh, const string filename, const aiScene *scene)
{
    int num_indices = 0;
    int num_vertices = mesh->mNumVertices;

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        num_indices += mesh->mFaces[i].mNumIndices;
    }
    
    vertex_t* vertices = (vertex_t*) calloc(num_vertices, sizeof(vertex_t));
    GLuint* indices = (unsigned int*) calloc(num_indices, sizeof(GLuint));

    for(unsigned int i = 0; i < num_vertices; i++)
    {
        vertices[i].Position  = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertices[i].Normal    = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);
        vertices[i].TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
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

    // Get Material Color
    aiColor3D color;
    scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    float opacity;
    scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_OPACITY, opacity);
    glm::vec4 color_vec4(color.r, color.g, color.b, opacity);

    string directory = filename.substr(0, filename.find_last_of('/'));

    // For now just try to load "{materialname}_diffuse.png" for the texture.
    aiString name;
    scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_NAME, name);
    string name_str = string(name.data);

    // Attempt to automatically cut off extension if it exists
    // Advise people not to put periods in material names.
    if (name_str.find('.')!=string::npos) name_str = name_str.substr(0, name_str.find_last_of('.'));

    string diffuse_filename = directory + "/" + name_str + "_diffuse.png";
    string normal_filename = directory + "/" + name_str + "_normal.png";
    string specular_filename = directory + "/" + name_str + "_specular.png";

    GLuint diffuse_texture = SOIL_load_OGL_texture(diffuse_filename.c_str(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    GLuint normal_texture = SOIL_load_OGL_texture(normal_filename.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    GLuint specular_texture = SOIL_load_OGL_texture(specular_filename.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

    if (diffuse_texture <= 0) fprintf(stderr, "ERROR::UNABLE TO LOAD DIFFUSE TEXTURE %s\n", diffuse_filename.c_str());
    //if (normal_texture <= 0) fprintf(stderr, "ERROR::UNABLE TO LOAD NORMAL TEXTURE %s\n", normal_filename.c_str());
    //if (specular_texture <= 0) fprintf(stderr, "ERROR::UNABLE TO LOAD SPECULAR TEXTURE %s\n", specular_filename.c_str());

    mesh_t result_mesh = gfx_create_mesh(num_indices, indices, num_vertices, vertices, color_vec4, diffuse_texture, normal_texture, specular_texture);

    free(vertices);
    free(indices);

    return result_mesh;
}