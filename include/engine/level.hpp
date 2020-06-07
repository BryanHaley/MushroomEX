#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "level_data.hpp"
#include <string>

level_t load_level_from_file(int *error_code, std::string filename);
int load_level_models(level_t *level, std::vector<std::string> geometry_files);
int load_level_collision(level_t *level, std::vector<std::string> collision_files);

void update_level();
void draw_level();

void unload_level();

std::vector<collision_surface_t> create_collision_surfaces_from_file(int *error_code, const std::string filename);

#endif