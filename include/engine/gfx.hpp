#ifndef MODEL_HANDLER_HPP
#define MODEL_HANDLER_HPP

#include <GL/gl3w.h>
#include "engine/gfx_data.hpp"

// TODO: put these into some sort of global settings struct
extern unsigned int g_ScrWidth, g_ScrHeight;
extern float g_FOV, g_Near, g_Far;
extern bool g_Fullscreen;

// TODO: put these into some sort of global GFX struct
extern GLuint MISSING_TEXTURE_INDEX;
extern std::vector<model_t> g_LoadedModels;
extern std::vector<shader_t> g_LoadedShaders;
extern glm::mat4 g_ViewMatrix, g_ProjMatrix;
extern glm::vec3 g_WorldUpVector, g_WorldForwardVector, g_WorldRightVector;
extern float g_CurrentFrameTime, g_LastFrameTime, g_DeltaTime;

int gfx_init();
void gfx_draw();
void gfx_update_render_viewport(float width, float height, float FOV, float near, float far);

mesh_t gfx_create_mesh(size_t num_indices, GLuint* indices, size_t num_vertices, GLfloat* vertices, glm::vec4 color, GLuint diffuse_texture, GLuint normal_texture, GLuint specular_texture);
model_t gfx_create_model(int *error_code, const std::string filename);
shader_t gfx_create_shader(int *error_code, const std::string directory, const std::string filename, const std::string vShaderExtension, const std::string fShaderExtension);
void gfx_draw_model(model_t *model, glm::mat4 matrix);
void gfx_draw_mesh(mesh_t *mesh);

#endif