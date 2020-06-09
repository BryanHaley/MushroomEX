#include "engine/gfx.hpp"
#include "glm/glm.hpp"

// TODO: It'd be faster to load all the meshes into one big array rather than rendering each model's meshes individually
void gfx_draw_model(model_t *model, glm::mat4 matrix)
{
	shader_t *shader = NULL;
	for (size_t i = 0; i < model->meshes.size(); i++)
	{
		// TODO: See if this is causing cache misses
		if (model->meshes[i].m_Shader != shader)
		{
			shader = model->meshes[i].m_Shader;

			glUseProgram(shader->m_ShaderProgram);

			glUniformMatrix4fv(shader->u_ModelMatrix, 1, GL_FALSE, &matrix[0][0]);
			glUniformMatrix4fv(shader->u_ViewMatrix, 1, GL_FALSE, &g_ViewMatrix[0][0]);
			glUniformMatrix4fv(shader->u_ProjMatrix, 1, GL_FALSE, &g_ProjMatrix[0][0]);

			glUniform1i(shader->u_DiffuseTex, DIFFUSE_GLTEXTURE_LOC);
	        glUniform1i(shader->u_NormalTex, NORMAL_GLTEXTURE_LOC);
	        glUniform1i(shader->u_SpecularTex, SPECULAR_GLTEXTURE_LOC);
			
			glUniform4fv(shader->u_Color, 1, &model->meshes[i].m_Color[0]);
			glUniform1i(shader->u_ColorBlendMode, model->meshes[i].m_ColorBlendMode);
		}

		gfx_draw_mesh(&model->meshes[i]);
	}
}

void gfx_draw_mesh(mesh_t *mesh)
{
	glActiveTexture( GL_TEXTURE0+DIFFUSE_GLTEXTURE_LOC );
    glBindTexture( GL_TEXTURE_2D, mesh->m_Diffuse );
    glActiveTexture( GL_TEXTURE0+NORMAL_GLTEXTURE_LOC );
    glBindTexture( GL_TEXTURE_2D, mesh->m_Normal );
    glActiveTexture( GL_TEXTURE0+SPECULAR_GLTEXTURE_LOC );
    glBindTexture( GL_TEXTURE_2D, mesh->m_Specular );

	glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->m_NumIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}