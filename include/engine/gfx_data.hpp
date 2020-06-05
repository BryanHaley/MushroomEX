#ifndef MODEL_DATA_HPP
#define MODEL_DATA_HPP

#include <GL/gl3w.h>
#include <vector>
#include <string>
#include "glm/glm.hpp"

#define MISSING_TEXTURE "__missing__"
#define VERTEX_POSITION_ATTRIB_LOC 0
#define VERTEX_NORMAL_ATTRIB_LOC   1
#define VERTEX_TEXCOORD_ATTRIB_LOC 2

typedef struct
{
	GLuint shader_program;
} shader_t;

typedef struct
{
	std::vector<GLuint> textures;
	std::vector<std::string> tex_names;
} tex_map;

typedef struct
{
	GLuint VAO, VBO, EBO;
	size_t num_indices;
	GLuint diffuse, normal, specular;
	glm::vec3 color;
} mesh_t;

typedef struct
{
	std::vector<mesh_t> meshes;
} model_t;

#endif