#include "engine/level.hpp"
#include "engine/gfx.hpp"
#include "engine/error_return_types.h"

#include <string>

int load_level_from_file(int *error_code, std::string directory, std::string filename, std::string extension)
{
	return NO_ERR;
}

level_t load_level(int *error_code, std::vector<std::string> geometry_files, std::vector<std::string> collision_files)
{
	level_t level;

	for (size_t i = 0; i < geometry_files.size(); i++)
	{
		if (geometry_files[i].compare("") != 0)
		{
			// Decompose filename

			std::string directory = geometry_files[i].substr(0, geometry_files[i].find_last_of('/'));
			std::string filename  = geometry_files[i].substr(geometry_files[i].find_last_of('/')+1, geometry_files[i].find_last_of('.'));
			std::string type      = geometry_files[i].substr(geometry_files[i].find_last_of('.')+1);

			model_t model = gfx_create_model(error_code, directory, filename, type);

			if (*error_code != NO_ERR)
			{
				fprintf(stderr, "ERROR::LEVEL::Could not load geometry from file: %s\n", geometry_files[i].c_str());
				return level;
			}

			level.m_Geometry.push_back(model);
		}
	}

	for (size_t i = 0; i < collision_files.size(); i++)
	{
		if (collision_files[i].compare("") != 0)
		{
			// Decompose filename
			std::string directory = collision_files[i].substr(0, collision_files[i].find_last_of('/'));
			std::string filename  = collision_files[i].substr(collision_files[i].find_last_of('/')+1, collision_files[i].find_last_of('.'));
			std::string type      = collision_files[i].substr(collision_files[i].find_last_of('.')+1);

			std::vector<collision_surface_t> surfaces = create_collision_surfaces_from_file(error_code, directory, filename, type);

			if (*error_code != NO_ERR)
			{
				fprintf(stderr, "ERROR::LEVEL::Could not load collision model from file: %s\n", collision_files[i].c_str());
				return level;
			}

			for (size_t i = 0; i < surfaces.size(); i++) level.m_CollisionSurfaces.push_back(surfaces[i]);
		}
	}

	*error_code = NO_ERR;
	return level;
}

void update_level()
{

}

void draw_level()
{

}

void unload_level()
{

}