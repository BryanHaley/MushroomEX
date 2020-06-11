#ifndef DEBUG_SPECTATOR_HPP
#define DEBUG_SPECTATOR_HPP

#include "glm/glm.hpp"

void debug_spectator_init();
void debug_spectator_update(glm::vec3 wishMove, glm::vec2 wishLook, bool wishRay);

#endif