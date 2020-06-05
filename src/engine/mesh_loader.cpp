#include "engine/gfx_data.hpp"
#include "engine/gfx.hpp"

mesh_t create_mesh(size_t num_indices, GLuint* indices, size_t num_vertices, GLfloat* vertices, GLuint diffuse_texture, GLuint normal_texture, GLuint specular_texture)
{
	mesh_t mesh;

    // create VAO and bind it
    glGenVertexArrays(1, &(mesh.VAO));
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &(mesh.VBO));
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*((num_vertices*3)+(num_vertices*3)+(num_vertices*2)), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &(mesh.EBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*num_indices, indices, GL_STATIC_DRAW);

    // bind attributes to proper locations
    glEnableVertexAttribArray(VERTEX_POSITION_ATTRIB_LOC);	
    glVertexAttribPointer(VERTEX_POSITION_ATTRIB_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIB_LOC);	
    glVertexAttribPointer(VERTEX_NORMAL_ATTRIB_LOC, 3, GL_FLOAT, GL_FALSE, 0, (void*) ((num_vertices*3)*sizeof(GLfloat)));

    glEnableVertexAttribArray(VERTEX_TEXCOORD_ATTRIB_LOC);	
    glVertexAttribPointer(VERTEX_TEXCOORD_ATTRIB_LOC, 2, GL_FLOAT, GL_FALSE, 0, (void*) (((num_vertices*3)+(num_vertices*3))*sizeof(GLfloat)));

    glBindVertexArray(0);

    mesh.color = glm::vec3(1.0f, 1.0f, 1.0f);
    mesh.diffuse = diffuse_texture;
    mesh.normal = normal_texture;
    mesh.specular = specular_texture;
    mesh.num_indices = num_indices;

	return mesh;
}