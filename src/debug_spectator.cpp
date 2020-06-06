#include "debug_spectator.hpp"
#include "engine/gfx.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

glm::vec3 g_dbgspecPosition, g_dbgspecForward, g_dbgspecRight, g_dbgspecUp;

void debug_spectator_init()
{
	g_dbgspecPosition = glm::vec3(0.0f);
	g_dbgspecRight 	  = glm::vec3(1.0f, 0.0f, 0.0f);
	g_dbgspecForward  = glm::vec3(0.0f, 0.0f, 1.0f);
}

void debug_spectator_update(glm::vec3 wishMove, glm::vec2 wishLook)
{
	// Rotate around world up axis (yaw)
	glm::mat4 rotationMatrix(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, wishLook.x, g_WorldUpVector);

	g_dbgspecForward = glm::vec3(glm::vec4(g_dbgspecForward, 1.0f) * rotationMatrix);
	g_dbgspecForward = glm::normalize(g_dbgspecForward);

	g_dbgspecRight = glm::cross(g_dbgspecForward, g_WorldUpVector);
	g_dbgspecRight = glm::normalize(g_dbgspecRight);

	// Rotate around local right axis (pitch)
	rotationMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, wishLook.y, g_dbgspecRight);

	g_dbgspecForward = glm::vec3(glm::vec4(g_dbgspecForward, 1.0f) * rotationMatrix);
	g_dbgspecForward = glm::normalize(g_dbgspecForward);

	g_dbgspecRight = glm::cross(g_dbgspecForward, g_WorldUpVector);
	g_dbgspecRight = glm::normalize(g_dbgspecRight);

	// Update position relative to new look direction
	g_dbgspecPosition = g_dbgspecPosition + (g_dbgspecRight   * wishMove.x);
	g_dbgspecPosition = g_dbgspecPosition + (g_WorldUpVector  * wishMove.y);
	g_dbgspecPosition = g_dbgspecPosition + (g_dbgspecForward * wishMove.z);

	// Update view matrix with new camera position
	g_ViewMatrix = glm::lookAt(g_dbgspecPosition, g_dbgspecPosition + g_dbgspecForward, g_WorldUpVector);
}
