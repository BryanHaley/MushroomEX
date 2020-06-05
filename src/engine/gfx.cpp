#include <vector>
#include "engine/gfx.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

std::vector<model_t> g_LoadedModels;
glm::mat4 view_matrix, projection_matrix;

bool gfx_init()
{
	view_matrix = glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,1.0f,0.0f));
	projection_matrix = glm::perspective(glm::radians(90.0f), 800.0f/600.0f, 1.0f, 1000.0f);

	int error_code;
	g_LoadedModels.push_back(create_model(&error_code, "mnt/mush/models/backpack", "backpack", "obj"));

	return true;
}

glm::mat4 gfx_get_vp_matrix()
{
	return view_matrix * projection_matrix;
}