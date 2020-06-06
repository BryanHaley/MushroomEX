#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "engine/error_return_types.h"
#include "engine/gfx.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#define OPENGL_MAJOR 3
#define OPENGL_MINOR 3

// Provide sensible defaults
unsigned int g_ScrWidth = 1280;
unsigned int g_ScrHeight = 720;
float g_FOV = 90.0f;
float g_Near = 0.1f;
float g_Far = 1000.0f;
bool g_Fullscreen = false;
float g_CurrentFrameTime = 0, g_LastFrameTime = 0, g_DeltaTime = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(g_ScrWidth, g_ScrHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "ERROR::GLFW::Could not create GLFW window.\n");
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
        fprintf(stderr, "ERROR::GL3W::Could not initialize GL3W. Got error code: %d\n", gl3w_init);
		return GL3W_ERR;
    }

    if (gfx_init() != NO_ERR) return GFX_ERR;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        g_CurrentFrameTime = glfwGetTime();
        g_DeltaTime = g_CurrentFrameTime - g_LastFrameTime;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model_matrix(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 5.0f));
        model_matrix = glm::rotate(model_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        gfx_draw_model(&g_LoadedModels[0], model_matrix);
 
        glfwSwapBuffers(window);
        glfwPollEvents();

        g_LastFrameTime = g_CurrentFrameTime;
    }

    glfwTerminate();
    return NO_ERR;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    gfx_update_render_viewport(width, height, g_FOV, g_Near, g_Far);
}

