#include "yaml-cpp/yaml.h"

#include <string>
#include <iostream>

#include "engine/level.hpp"
#include "engine/gfx.hpp"
#include "engine/error_return_types.h"

level_t load_level_from_file(int *error_code, std::string filename)
{
    level_t level;

    YAML::Node level_yaml = YAML::LoadFile(filename);

    if (!level_yaml["scene"])
    {
        fprintf(stderr, "ERROR::LEVEL::Unable to load level from file: %s\n", filename.c_str());
        *error_code = FILE_IO_ERR;
        return level;
    }

    // Test
    std::vector<std::string> level_geo_models;
    level_geo_models.push_back(level_yaml["scene"]["objects"]["level_geo"]["file"].as<std::string>());
    *error_code = load_level_models(&level, level_geo_models);
    if (*error_code != NO_ERR) return level;

    std::vector<std::string> level_collision_files;
    level_collision_files.push_back(level_yaml["scene"]["objects"]["level_collision"]["file"].as<std::string>());
    *error_code = load_level_collision(&level, level_collision_files);
    if (*error_code != NO_ERR) return level;

	return level;
}

int load_level_models(level_t *level, std::vector<std::string> geometry_files)
{
    int error_code = NO_ERR;

	for (size_t i = 0; i < geometry_files.size(); i++)
	{
		if (geometry_files[i].compare("") != 0)
		{
			// Decompose filename
			std::string directory = geometry_files[i].substr(0, geometry_files[i].find_last_of('/'));
			std::string filename  = geometry_files[i].substr(geometry_files[i].find_last_of('/')+1, geometry_files[i].find_last_of('.')-geometry_files[i].find_last_of('/')-1);
			std::string type      = geometry_files[i].substr(geometry_files[i].find_last_of('.')+1);

			model_t model = gfx_create_model(&error_code, "mnt/" + directory, filename, type);

			if (error_code != NO_ERR)
			{
				fprintf(stderr, "ERROR::LEVEL::Could not load geometry from file: %s\n", geometry_files[i].c_str());
				return FILE_IO_ERR;
			}

			level->m_Geometry.push_back(model);
		}
	}

	return error_code;
}

int load_level_collision(level_t *level, std::vector<std::string> collision_files)
{
    int error_code = NO_ERR;

    for (size_t i = 0; i < collision_files.size(); i++)
    {
        if (collision_files[i].compare("") != 0)
        {
            create_collision_surfaces_from_file(&error_code, &level->m_CollisionSurfaces, "mnt/" + collision_files[i]);
            if (error_code != NO_ERR)
            {
                fprintf(stderr, "ERROR::LEVEL::Could not load collision model from file: %s\n", collision_files[i].c_str());
                return FILE_IO_ERR;
            }
        }
    }

    return error_code;
}

void update_level(level_t* level)
{

}

void draw_level(level_t* level)
{

}

void unload_level(level_t* level)
{

}