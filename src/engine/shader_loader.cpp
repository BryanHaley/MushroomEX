#include <fstream>
#include <streambuf>
#include <string>
#include <algorithm>
#include <iterator>
#include <cerrno>
#include <stdio.h>

#include "engine/gfx.hpp"
#include "engine/error_return_types.h"

std::string get_string_from_file(int *error_code, const std::string filename);

shader_t gfx_create_shader(int *error_code, const std::string directory, const std::string filename, const std::string vShaderExtension, const std::string fShaderExtension)
{
	shader_t shader;

	std::string vShader_filename = filename + "." + vShaderExtension;
	std::string fShader_filename = filename + "." + fShaderExtension;

	std::string vShader_code = get_string_from_file(error_code, directory + "/" + vShader_filename) + "\0";
	if (*error_code != NO_ERR) return shader;
	std::string fShader_code = get_string_from_file(error_code, directory + "/" + fShader_filename) + "\0";
	if (*error_code != NO_ERR) return shader;

	// build and compile our shader program
    int success;
    GLchar infoLog[1024] = {'\0'};

    // compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vShader_code_c_str = vShader_code.c_str();
    glShaderSource(vertexShader, 1, &vShader_code_c_str, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        fprintf(stderr, "ERROR::VERTEX SHADER::Could not compile %s.\n--OPENGL LOG--\n%s\n--END OPENGL LOG--\n\n\n", vShader_filename.c_str(), infoLog);
        *error_code = SHADER_ERR;
        return shader;
    }

    // compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fShader_code_c_str = fShader_code.c_str();
    glShaderSource(fragmentShader, 1, &fShader_code_c_str, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        fprintf(stderr, "ERROR::FRAGMENT SHADER::Could not compile %s.\n--OPENGL LOG--\n%s\n--END OPENGL LOG--\n\n\n", fShader_filename.c_str(), infoLog);
        *error_code = SHADER_ERR;
        return shader;
    }

    // link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER PROGRAM::Could not link shader %s.\n--OPENGL LOG--\n%s\n--END OPENGL LOG--\n\n\n", filename.c_str(), infoLog);
        *error_code = SHADER_ERR;
        return shader;
    }

    // clean up vertex and frag shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    shader.m_Name = filename;
    shader.m_ShaderProgram = shaderProgram;

    shader.u_ModelMatrix = glGetUniformLocation(shader.m_ShaderProgram, "u_ModelMatrix");
    shader.u_ViewMatrix = glGetUniformLocation(shader.m_ShaderProgram, "u_ViewMatrix");
    shader.u_ProjMatrix = glGetUniformLocation(shader.m_ShaderProgram, "u_ProjMatrix");

    shader.u_DiffuseTex = glGetUniformLocation(shader.m_ShaderProgram, "u_DiffuseTex");
    shader.u_NormalTex = glGetUniformLocation(shader.m_ShaderProgram, "u_NormalTex");
    shader.u_SpecularTex = glGetUniformLocation(shader.m_ShaderProgram, "u_SpecularTex");

    shader.u_Color = glGetUniformLocation(shader.m_ShaderProgram, "u_Color");
    shader.u_ColorBlendMode = glGetUniformLocation(shader.m_ShaderProgram, "u_ColorBlendMode");

    *error_code = NO_ERR;
    return shader;
}

std::string get_string_from_file(int *error_code, const std::string filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		*error_code = NO_ERR;
		return(contents);
	}
	else
	{
		printf("ERROR::FILE::Could not find file: %s\n", filename.c_str());
		*error_code = FILE_IO_ERR;
		return "";
	}
}