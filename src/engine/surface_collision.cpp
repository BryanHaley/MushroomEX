#include "engine/surface_collision.hpp"

using namespace glm;
using namespace SurfaceCollision;

// Reference: https://github.com/johnnovak/raytriangle-test/blob/master/cpp/perftest.cpp
bool SurfaceCollision::Raycast (vec3 origin, vec3 dir, vec3 &out, vec3 v0, vec3 v1, vec3 v2)
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
    out = origin + (dir*dist);
    return true;
}

bool SurfaceCollision::Raycast (glm::vec3 origin, glm::vec3 dir, glm::vec3 &out, Scene::scene_t* scene)
{
    for (size_t i = 0; i < scene->CollisionSurfaces.size(); i++)
    {
        if (SurfaceCollision::Raycast(origin, dir, out,
            scene->CollisionSurfaces[i].v0, scene->CollisionSurfaces[i].v1, scene->CollisionSurfaces[i].v2))
            return true;
    }

    return false;
}