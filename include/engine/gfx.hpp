#ifndef MODEL_HANDLER_HPP
#define MODEL_HANDLER_HPP

#include <GL/gl3w.h>
#include "engine/gfx_data.hpp"

extern GLuint MISSING_TEXTURE_INDEX;
extern std::vector<model_t> g_LoadedModels;
extern glm::mat4 view_matrix, projection_matrix, vp_matrix;

mesh_t create_mesh(size_t num_indices, GLuint* indices, size_t num_vertices, GLfloat* vertices, GLuint texture);
model_t create_model(int *error, const std::string directory, const std::string filename, const std::string type);
void draw_model(model_t *model);
void draw_mesh(mesh_t *mesh);

bool gfx_init();
void gfx_draw();
glm::mat4 gfx_get_vp_matrix();

#endif