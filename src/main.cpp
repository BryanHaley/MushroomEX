#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "engine/error_return_types.h"
#include "engine/utils.hpp"
#include "engine/gfx.hpp"
#include "game/scene.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#ifdef DEBUG_SPECTATOR
#include "debug_spectator.hpp"
#endif

#define OPENGL_MAJOR 3
#define OPENGL_MINOR 3

// Provide sensible defaults
unsigned int g_ScrWidth = 1280;
unsigned int g_ScrHeight = 720;
float g_FOV = 45.0f;
float g_Near = 0.1f;
float g_Far = 1000.0f;
bool g_Fullscreen = false;
float g_CurrentFrameTime = 0, g_LastFrameTime = 0, g_DeltaTime = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    int error_code;

    // glfw: initialize and configure ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef MACOS_BUILD
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(g_ScrWidth, g_ScrHeight, "MushroomEX", NULL, NULL);
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

    #ifdef DEBUG_SPECTATOR
    debug_spectator_init();
    #endif

    Scene::scene_t scene;
    Scene::Init(&scene);
    Scene::LoadFromFile(&error_code, &scene, "mnt/base/scenes/bob/bob.scene.yaml");
    if (error_code != NO_ERR) return error_code;
    Scene::Start(&scene);
    if (error_code != NO_ERR) return error_code;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        g_CurrentFrameTime = glfwGetTime();
        g_DeltaTime = g_CurrentFrameTime - g_LastFrameTime;

        glfwPollEvents();
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Test
        Scene::Update(&scene);
        Scene::Draw  (&scene);
 
        glfwSwapBuffers(window);

        g_LastFrameTime = g_CurrentFrameTime;
    }

    Scene::Unload(&scene);
    glfwTerminate();
    return NO_ERR;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    #ifdef DEBUG_SPECTATOR

    glm::vec3 wishMove(0.0f);
    if      (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) wishMove.z =  10.0f * g_DeltaTime;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) wishMove.z = -10.0f * g_DeltaTime;
    if      (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) wishMove.x =  10.0f * g_DeltaTime;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) wishMove.x = -10.0f * g_DeltaTime;
    if      (glfwGetKey(window, GLFW_KEY_SPACE)        == GLFW_PRESS) wishMove.y =  10.0f * g_DeltaTime;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) wishMove.y = -10.0f * g_DeltaTime;

    //if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) wishMove = wishMove * 2;

    glm::vec2 wishLook(0.0f);
    if      (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) wishLook.x = glm::radians( 50.0f * g_DeltaTime);
    else if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) wishLook.x = glm::radians(-50.0f * g_DeltaTime);
    if      (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) wishLook.y = glm::radians(-50.0f * g_DeltaTime);
    else if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) wishLook.y = glm::radians( 50.0f * g_DeltaTime);

    debug_spectator_update(wishMove, wishLook);

    #endif
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    gfx_update_render_viewport(width, height, g_FOV, g_Near, g_Far);
}

