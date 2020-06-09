#include <GL/gl3w.h>
#include "glm/glm.hpp"
#include "engine/gfx_data.hpp"
#include "engine/gfx.hpp"

mesh_t gfx_create_mesh(size_t num_indices, GLuint* indices, size_t num_vertices, vertex_t *vertices, glm::vec4 color, GLuint diffuse_texture, GLuint normal_texture, GLuint specular_texture)
{
	mesh_t mesh;

    // create VAO and bind it
    glGenVertexArrays(1, &(mesh.VAO));
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &(mesh.VBO));
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(vertex_t), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &(mesh.EBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*num_indices, indices, GL_STATIC_DRAW);

    // bind attributes to proper locations 
    // Positions
    glEnableVertexAttribArray(VERTEX_POSITION_ATTRIB_LOC);   
    glVertexAttribPointer(VERTEX_POSITION_ATTRIB_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIB_LOC);   
    glVertexAttribPointer(VERTEX_NORMAL_ATTRIB_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(VERTEX_TEXCOORD_ATTRIB_LOC);   
    glVertexAttribPointer(VERTEX_TEXCOORD_ATTRIB_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, TexCoords));

    glBindVertexArray(0);

    mesh.m_Shader = &g_LoadedShaders[DEFAULT_SHADER_INDEX];
    mesh.m_NumIndices = num_indices;
    mesh.m_Diffuse = diffuse_texture;
    mesh.m_Normal = normal_texture;
    mesh.m_Specular = specular_texture;
    mesh.m_Color = color;
    // Discard colors for now. TODO: support some sort of material file for setting color blending modes
    mesh.m_ColorBlendMode = CB_DISCARD;

	return mesh;
}