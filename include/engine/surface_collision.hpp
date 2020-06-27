#ifndef SURFACE_COLLISION_HPP
#define SURFACE_COLLISION_HPP

#include "game/scene.hpp"
#include "glm/glm.hpp"

namespace SurfaceCollision
{
    typedef struct
    {
        collision_surface_t surface;
        glm::vec3 positionOnSurface;
        float surfaceThickness;
        float distanceFromSurface;
    } collision_t;

    bool LineSegIntersectsTriangle (glm::vec3 lineSegA, glm::vec3 lineSegB, glm::vec3 triA, glm::vec3 triB, glm::vec3 triC);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit_pos, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit_pos, glm::mat4 matrix, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, Scene::scene_t &scene);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit_pos, Scene::scene_t &scene);
    bool Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit_pos, collision_surface_t *hit, Scene::scene_t &scene);

    void CacheSurfaceInfo (collision_surface_t &surface);
    void CacheSurfacesInfo(std::vector<collision_surface_t> &surfaces);

    void FindSurfaceCollisions (
        // input
        Scene::scene_t &scene, const glm::vec3 &position, const float radius, const float height,
        const std::vector<collision_surface_t> &surfaces,
        // output
        std::vector<collision_t> &wallCollisions,
        std::vector<collision_t> &floorCollisions,
        std::vector<collision_t> &ceilingCollisions,
        glm::vec3 *posOnFloor,
        bool *isInsideLevel,
        bool *isAboveFloor,
        // control
        bool onlyCheckWalls );

    glm::vec3 GetWallCollisionVector  (const glm::vec3 &position, const std::vector<collision_t> &wallCollisions);
    glm::vec3 GetFloorCollisionVector (const glm::vec3 &position, const std::vector<collision_t> &floorCollisions);
}

#endif