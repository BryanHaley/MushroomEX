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

#define DIFFUSE_GLTEXTURE_LOC 0
#define NORMAL_GLTEXTURE_LOC 1
#define SPECULAR_GLTEXTURE_LOC 2

#define DEFAULT_SHADER_INDEX 0

typedef struct
{
	std::string m_Name;
	GLuint m_ShaderProgram;
	GLuint u_ModelMatrix, u_ViewMatrix, u_ProjMatrix;
	GLuint u_DiffuseTex, u_NormalTex, u_SpecularTex;
} shader_t;

typedef struct
{
	std::vector<GLuint> m_Textures;
	std::vector<std::string> m_Names;
} tex_map;

typedef struct
{
	shader_t *m_Shader;
	GLuint VAO, VBO, EBO;
	size_t m_NumIndices;
	GLuint m_Diffuse, m_Normal, m_Specular;
	glm::vec3 m_Color;
} mesh_t;

typedef struct
{
	std::vector<mesh_t> meshes;
} model_t;

#endif