#include <vector>
#include <GL/gl3w.h>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "engine/gfx.hpp"
#include "engine/error_return_types.h"

std::vector<model_t> g_LoadedModels;
std::vector<shader_t> g_LoadedShaders;
glm::vec3 g_WorldUpVector, g_WorldForwardVector, g_WorldRightVector;
glm::mat4 g_ViewMatrix, g_ProjMatrix;

int gfx_init()
{
	fprintf(stderr, "OpenGL version is (%s)\n\n", glGetString(GL_VERSION));

	g_WorldRightVector   = glm::vec3(1.0f, 0.0f, 0.0f);
	g_WorldUpVector 	 = glm::vec3(0.0f, 1.0f, 0.0f);
	g_WorldForwardVector = glm::vec3(0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	// Set up default view and projection matrices
	g_ViewMatrix = glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,1.0f,0.0f));
	gfx_update_render_viewport(g_ScrWidth, g_ScrHeight, g_FOV, g_Near, g_Far);

	// Load default shader
	int error_code;
	g_LoadedShaders.push_back(gfx_create_shader(&error_code, "mnt/mush/shaders", "default_mesh", "vert", "frag"));
	if (error_code != NO_ERR) return error_code;

	// Test
	g_LoadedModels.push_back(gfx_create_model(&error_code, "mnt/mush/models/bob", "bob", "obj"));
	if (error_code != NO_ERR) return error_code;

	return NO_ERR;
}

void gfx_update_render_viewport(float width, float height, float FOV, float near, float far)
{
	g_ProjMatrix = glm::perspective(glm::radians(FOV), width/height, near, far);
	glViewport(0, 0, width, height);

	g_ScrWidth = (unsigned int) width;
	g_ScrHeight = (unsigned int) height;
	g_FOV = FOV;
	g_Near = near;
	g_Far = far;
}