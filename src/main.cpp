#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "engine/error_return_types.h"
#include "engine/gfx.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#define OPENGL_MAJOR 3
#define OPENGL_MINOR 3

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 normal;\n"
    "layout (location = 2) in vec2 texcoord;\n"

    "out vec3 frag_position;\n"
    "out vec3 frag_normal;\n"
    "out vec2 frag_texcoord;\n"

    "uniform mat4 model_mat;\n"
    "uniform mat4 view_mat;\n"
    "uniform mat4 proj_mat;\n"

    "void main()\n"
    "{\n"
    "   frag_position = position;\n"
    "   frag_normal = normal;\n"
    "   frag_texcoord = texcoord;\n"

    "	vec4 transformed_pos = proj_mat * view_mat * model_mat * vec4(position, 1.0);\n"
    "   gl_Position = transformed_pos;\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
	"in vec3 frag_position;\n"
    "in vec3 frag_normal;\n"
    "in vec2 frag_texcoord;\n"

    "out vec4 FragColor;\n"

    "uniform sampler2D diffuse_texture;\n"
    "uniform sampler2D normal_texture;\n"
    "uniform sampler2D specular_texture;\n"

    "void main()\n"
    "{\n"
    "   FragColor = texture(diffuse_texture, frag_texcoord);\n"
    "}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "ERROR::Could not create GLFW window.\n");
        glfwTerminate();
        return GLFW_ERR;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    int gl3w_init = gl3wInit();
    if (gl3w_init != NO_ERR)
    {
        fprintf(stderr, "ERROR::Could not initialize GL3W. Got error code: %d\n", gl3w_init);
		return GL3W_ERR;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint shader_model_mat_loc = glGetUniformLocation(shaderProgram, "model_mat");
    GLuint shader_view_mat_loc = glGetUniformLocation(shaderProgram, "view_mat");
    GLuint shader_proj_mat_loc = glGetUniformLocation(shaderProgram, "proj_mat");

    GLuint shader_diffuse_texture_loc = glGetUniformLocation(shaderProgram, "diffuse_texture");
    GLuint shader_normal_texture_loc = glGetUniformLocation(shaderProgram, "normal_texture");
    GLuint shader_specular_texture_loc = glGetUniformLocation(shaderProgram, "specular_texture");

    gfx_init();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        
        glm::mat4 model_matrix(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 5.0f));
        model_matrix = glm::rotate(model_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(shader_model_mat_loc, 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(shader_view_mat_loc, 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(shader_proj_mat_loc, 1, GL_FALSE, &projection_matrix[0][0]);

        glUniform1i(shader_diffuse_texture_loc, 0);
        glUniform1i(shader_normal_texture_loc, 1);
        glUniform1i(shader_specular_texture_loc, 2);

        draw_model(&g_LoadedModels[0]);
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    /*glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);*/

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

