#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "level_data.hpp"
#include <string>

int load_level_from_file(std::string directory, std::string filename, std::string extension);
level_t load_level(int *error_code, std::vector<std::string> geometry_files, std::vector<std::string> collision_files);

void update_level();
void draw_level();

void unload_level();

std::vector<collision_surface_t> create_collision_surfaces_from_file(int *error_code, const std::string directory, const std::string filename, const std::string type);

#endif