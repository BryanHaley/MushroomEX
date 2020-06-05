#include "engine/gfx.hpp"

void draw_model(model_t *model)
{
	for (size_t i; i < model->meshes.size(); i++)
	{
		draw_mesh(&model->meshes[i]);
	}
}

void draw_mesh(mesh_t *mesh)
{
	glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}