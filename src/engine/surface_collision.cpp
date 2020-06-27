#include "engine/surface_collision.hpp"

#include <math.h>
#include <vector>
#include <limits>

using namespace glm;
using namespace SurfaceCollision;

// Reference: https://github.com/johnnovak/raytriangle-test/blob/master/cpp/perftest.cpp
bool SurfaceCollision::Raycast (vec3 origin, vec3 dir, vec3 *hit_pos, vec3 v0, vec3 v1, vec3 v2)
{
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;

    vec3 pvec = glm::cross(dir, v0v2);
    float det = glm::dot(v0v1, pvec);

    if (det < 0.000001)
    return false;

    float invDet = 1.0 / det;

    vec3 tvec = origin - v0;
    float u = glm::dot(tvec, pvec) * invDet;

    if (u < 0 || u > 1)
    return false;

    vec3 qvec = glm::cross(tvec, v0v1);

    float v = glm::dot(dir, qvec) * invDet;

    if (v < 0 || u + v > 1)
    return false;

    float dist = glm::dot(v0v2, qvec) * invDet;
    if (hit_pos != NULL) *hit_pos = origin + (dir*dist);
    return true;
}

bool SurfaceCollision::Raycast (vec3 origin, vec3 dir, vec3 *hit_pos, mat4 matrix, vec3 v0, vec3 v1, vec3 v2)
{
    glm::vec4 v0_4 = matrix * glm::vec4(v0, 1.0f);
    glm::vec4 v1_4 = matrix * glm::vec4(v1, 1.0f);
    glm::vec4 v2_4 = matrix * glm::vec4(v2, 1.0f);
    v0 = glm::vec3(v0_4.x, v0_4.y, v0_4.z);
    v1 = glm::vec3(v1_4.x, v1_4.y, v1_4.z);
    v2 = glm::vec3(v2_4.x, v2_4.y, v2_4.z);

    return SurfaceCollision::Raycast (origin, dir, hit_pos, v0, v1, v2);
}

bool SurfaceCollision::Raycast (glm::vec3 origin, glm::vec3 dir, Scene::scene_t &scene)
{
    return SurfaceCollision::Raycast (origin, dir, NULL, NULL, scene);
}

bool SurfaceCollision::Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit_pos, Scene::scene_t &scene)
{
    return SurfaceCollision::Raycast (origin, dir, hit_pos, NULL, scene);
}

bool SurfaceCollision::Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit_pos, collision_surface_t *hit, Scene::scene_t &scene)
{
    for (size_t i = 0; i < scene.CollisionSurfaces.size(); i++)
    {
        glm::mat4 transformationMatrix = scene.Transforms[scene.CollisionSurfaces[i].gObjIndex];

        if (SurfaceCollision::Raycast(origin, dir, hit_pos, transformationMatrix, 
            scene.CollisionSurfaces[i].v0, 
            scene.CollisionSurfaces[i].v1, 
            scene.CollisionSurfaces[i].v2))
        {
            hit = &scene.CollisionSurfaces[i];
            return true;
        }
    }

    return false;
}

void SurfaceCollision::CacheSurfaceInfo (collision_surface_t &surface)
{
    // Reference for implementation: https://www.youtube.com/watch?v=UnU7DJXiMAQ

    // Cache if the surface is a wall, floor, or ceiling.
    if      (surface.normal.y > 0.01f)  surface.flags &= SURFACE_FLOOR;
    else if (surface.normal.y < -0.01f) surface.flags &= SURFACE_CEILING;
    else                                surface.flags &= SURFACE_WALL;

    if (surface.flags & SURFACE_WALL)
    {
        // Cache if the wall faces the z or x axis more.
        if (abs(surface.normal.z) > abs(surface.normal.x))
        surface.flags &= SURFACE_FACING_Z;
    }
}

void SurfaceCollision::CacheSurfacesInfo (std::vector<collision_surface_t> &surfaces)
{
    for (int i = 0; i < surfaces.size(); i++)
    {
        if (surfaces[i].flags & SURFACE_STATIC) continue;
        else SurfaceCollision::CacheSurfaceInfo(surfaces[i]);
    }
}

// TODO: It's probably worth it to expose collision surfaces to AngelScript so this can be replaced with custom logic.
void SurfaceCollision::FindSurfaceCollisions (
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
        bool onlyCheckWalls )
{
    // Reference for implementation: https://www.youtube.com/watch?v=UnU7DJXiMAQ

    // Note: we could just check for the first surface on each direction of each axis we collide with and that would
    // give us a decent enough approximation of Mario 64's collision handling; however it is possible to collide with
    // an arbitrary number of surfaces on each axis at any given time in Mario 64 (usually in the case of corners and
    // such), so I've done it that way as this nuance might affect speedrunning techniques.

    // TODO: Maybe add an option to only check for the nearest surface for performance reasons. NPCs probably don't
    // need to do the more advanced check, for example.

    // Find if the surface immediately below us is a floor. If so, ceilings below us should be ignored. Most character
    // controllers will also want this information to snap the character to the floor, so allow it to be gathered from
    // this function rather than recalculated.
    bool ignoreLowerCeilings = false;
    glm::vec3 floor_hit_pos;
    collision_surface_t hit;
    if (SurfaceCollision::Raycast(position, glm::vec3(0, -1, 0), &floor_hit_pos, &hit, scene))
    {
        if (hit.flags & SURFACE_FLOOR)
        {
            ignoreLowerCeilings = true;
            if (isAboveFloor != NULL) *isAboveFloor = true;
            if (posOnFloor != NULL) *posOnFloor = floor_hit_pos;
        }
    }

    // If there's nothing at all below us, the object is outside of the level, but continue with the rest of the func
    // as this might be fine for non-Mario objects or custom player controllers.
    else
    {
        if (isInsideLevel != NULL) *isInsideLevel = false;
    }

    // Check through all surfaces in a scene.
    for (int i = 0; i < surfaces.size(); i++)
    {
        collision_t collision;
        collision.surface = surfaces[i];
        float distance = -1;

        // Get transformed surface vertices
        glm::mat4 surfaceTransformationMatrix = scene.Transforms[surfaces[i].gObjIndex];
        glm::vec4 v0_4 = surfaceTransformationMatrix * glm::vec4(surfaces[i].v0, 1.0f);
        glm::vec4 v1_4 = surfaceTransformationMatrix * glm::vec4(surfaces[i].v1, 1.0f);
        glm::vec4 v2_4 = surfaceTransformationMatrix * glm::vec4(surfaces[i].v2, 1.0f);
        glm::vec3 v0 = glm::vec3(v0_4.x, v0_4.y, v0_4.z);
        glm::vec3 v1 = glm::vec3(v1_4.x, v1_4.y, v1_4.z);
        glm::vec3 v2 = glm::vec3(v2_4.x, v2_4.y, v2_4.z);

        // Quickly eliminate surfaces that are out of range vertically
        if ((position.y > v0.y+height+radius && 
             position.y > v1.y+height+radius && 
             position.y > v2.y+height+radius) ||
            (position.y < v0.y-(height+radius) && 
             position.y < v1.y-(height+radius) && 
             position.y < v2.y-(height+radius)) )
        {
            continue;
        }

        // Most surfaces will be floors, so check for that first.
        if (!onlyCheckWalls && surfaces[i].flags & SURFACE_FLOOR)
        {
            //For Mario's height of 144 units in Mario 64, this (approximately) gives us the original floor thickness 
            //value of 78 units while remaining generic to characters of any height.
            collision.surfaceThickness = (height/2)+(height*0.042f);

            // Find our projected position on the floor
            glm::vec3 positionOnFloor;
            if (SurfaceCollision::Raycast(position, glm::vec3(0,1,0), &positionOnFloor, v0, v1, v2))
            {
                // Check if we're within (height/2)+(height*0.042f) units under the floor.
                distance = glm::distance(position, positionOnFloor);
                if (distance < collision.surfaceThickness)
                {
                    // In practice this means if Mario (or whomever) is clipping less than halfway through the floor,
                    // they will be considered "colliding" with the floor.
                    collision.positionOnSurface = positionOnFloor;
                    collision.distanceFromSurface = distance;
                    floorCollisions.push_back(collision);
                }
            }

            // If no raycast hit, we're either above the floor or are outside of the triangle
            else continue;
        }

        // Now check ceilings
        else if (!onlyCheckWalls && surfaces[i].flags & SURFACE_CEILING)
        {
            // Check if we're below the ceiling
            glm::vec3 positionOnCeiling;
            if (SurfaceCollision::Raycast(position, glm::vec3(0,1,0), &positionOnCeiling, v0, v1, v2))
            {
                // If we are, check if we're more than height units under the floor.
                if (glm::distance(position, positionOnCeiling) > height) continue;
            }

            // Otherwise make sure we're actually above the ceiling
            if (!SurfaceCollision::Raycast(position, glm::vec3(0,-1,0), &positionOnCeiling, v0, v1, v2)) continue;

            distance = glm::distance(position, positionOnCeiling);

            // If we are, check if there's a floor between us and the ceiling
            if (ignoreLowerCeilings)
            {
                float distanceFromFloor = glm::distance(position, floor_hit_pos);
                if (distanceFromFloor < distance && floor_hit_pos.y > positionOnCeiling.y) continue;
            }

            // Otherwise, we're colliding with the ceiling
            collision.surfaceThickness = height;
            collision.distanceFromSurface = distance;
            collision.positionOnSurface = positionOnCeiling;
            ceilingCollisions.push_back(collision);
        }

        // Finally, check walls.
        else if (surfaces[i].flags & SURFACE_WALL && surfaces[i].flags & SURFACE_FACING_Z)
        {
            // In original SM64, there is a 30 unit gap at the top of walls that Mario does not collide with. We want
            // this, except based on the object's height. 30 units divided by Mario's original height = approx 0.208,
            // this we want to test if we're less than (height*0.208) units from the top.
            float wallTallestPoint = fmax(fmax(v0.y, v1.y), v2.y);
            if (wallTallestPoint - position.y < height*0.208f) continue;

            // In original SM64, there is another 30 unit space from the top of the wall where Mario can get roughly
            // twice as close to the wall as he usually can. Thus, we want half of the original radius in this case.
            float adjustedRadius = radius;
            if (wallTallestPoint - position.y < height*0.417f) adjustedRadius = radius/2;

            collision.surfaceThickness = adjustedRadius;

            // TODO: Those things change depending on Mario's state (on ground, in air, swimming, flying). Account for
            // this gracefully somehow. Maybe pass a list of wall hitboxes defined by distance from the top and
            // thickness into this function. Floors and ceilings should be fine to leave default however; snapping to
            // the ground can be done without needing to further complicate this function.

            bool foundWallCollision;
            glm::vec3 positionOnWall;

            if (surfaces[i].flags & SURFACE_FACING_Z)
            {
                // Find our projected position on the z-facing wall
                foundWallCollision = SurfaceCollision::Raycast(position, glm::vec3(0,0,1),  &positionOnWall, v0, v1, v2);
                foundWallCollision = SurfaceCollision::Raycast(position, glm::vec3(0,0,-1), &positionOnWall, v0, v1, v2);
            }

            else
            {
                // Find our projected position on the x-facing wall
                foundWallCollision = SurfaceCollision::Raycast(position, glm::vec3(1,0,0),  &positionOnWall, v0, v1, v2);
                foundWallCollision = SurfaceCollision::Raycast(position, glm::vec3(-1,0,0), &positionOnWall, v0, v1, v2);
            }

            if (!foundWallCollision) continue;

            distance = glm::distance(position, positionOnWall);

            // Check if we're within radius units of the wall.
            if (distance < adjustedRadius)
            {
                // In practice this means if Mario (or whomever) is clipping less than halfway into a wall, they will
                // be considered "colliding" with the wall.
                collision.distanceFromSurface = distance;
                collision.positionOnSurface = positionOnWall;
                floorCollisions.push_back(collision);
            }
        }
    }
}

// Reference for implementation: https://www.youtube.com/watch?v=UnU7DJXiMAQ&t=138s 
// Ceilings must be handled by the character controller as the don't directly "push" Mario anywhere.
glm::vec3 GetWallCollisionVector (const glm::vec3 &position, const std::vector<collision_t> &wallCollisions)
{
    if (wallCollisions.size() == 0) return glm::vec3(0.0f);

    glm::vec3 newPosition = position;

    for (int i = 0; i < wallCollisions.size(); i++)
    {
        // Find push vector
        glm::vec3 pushVector = glm::vec3(0.0f);
        if (wallCollisions[i].surface.flags & SURFACE_FACING_Z)
        {
            if (wallCollisions[i].surface.normal.z > 0) pushVector.z = 1;
            else pushVector.z = -1;
        }

        else if (wallCollisions[i].surface.normal.x > 0) pushVector.x = 1;
        else pushVector.x = -1;

        glm::vec3 pushPosition = wallCollisions[i].positionOnSurface + (pushVector*wallCollisions[i].surfaceThickness);
        newPosition += pushPosition-position;
    }

    return newPosition-position;
}

glm::vec3 GetFloorCollisionVector (const glm::vec3 &position, const std::vector<collision_t> &floorCollisions)
{
    if (floorCollisions.size() == 0) return glm::vec3(0.0f);

    glm::vec3 newPosition = glm::vec3(0.0f, std::numeric_limits<float>::min(), 0.0f);

    for (int i = 0; i < floorCollisions.size(); i++)
    {
        // New position becomes whichever floor is pushing the object highest
        if (floorCollisions[i].positionOnSurface.y > newPosition.y)
            newPosition = floorCollisions[i].positionOnSurface;
    }

    return newPosition-position;
}