#ifndef SURFACE_COLLISION_HPP
#define SURFACE_COLLISION_HPP

#include "game/scene.hpp"
#include "glm/glm.hpp"

namespace SurfaceCollision
{
    bool LineSegIntersectsTriangle (glm::vec3 lineSegA, glm::vec3 lineSegB, glm::vec3 triA, glm::vec3 triB, glm::vec3 triC);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 &out, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 &out, Scene::scene_t* scene);

    #define CalcSignedTetrahedronVolume(a, b, c, d) (1.0f/6.0f)*glm::dot(glm::cross(b-a, c-a), d-a)
}

#endif