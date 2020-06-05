#include "engine/gfx.hpp"

void draw_model(model_t *model)
{
	for (size_t i = 0; i < model->meshes.size(); i++)
	{
		draw_mesh(&model->meshes[i]);
	}
}

void draw_mesh(mesh_t *mesh)
{
	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, mesh->diffuse );
    glActiveTexture( GL_TEXTURE0+1 );
    glBindTexture( GL_TEXTURE_2D, mesh->normal );
    glActiveTexture( GL_TEXTURE0+2 );
    glBindTexture( GL_TEXTURE_2D, mesh->specular );

	glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}