#include "engine/utils.hpp"
#include "game/scene.hpp"

#include "glm/gtx/euler_angles.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace Utils;
using glm::vec3;
using glm::mat4;

// Why doesn't this work?? Linker errors of undefined symbols when I try to use it.
template <typename T>
void Utils::CheckIfVectorOutOfMemory(std::vector<T> *vec, int expectedSize)
{
    if (vec->size() != expectedSize)
    {
        fprintf(stderr, 
            "ERROR::OUT OF MEMORY::Failed to allocated additional memory for %s vector. Shutting down.\n", 
            typeid(T).name());
        abort();
    }
}

mat4 Utils::MatrixFromPosRot(float position[3], float euler[3])
{
    vec3 translation = vec3(position[0], position[1], position[2]);
    mat4 matrix = glm::eulerAngleYXZ(euler[0], euler[1], euler[2]);
    matrix = glm::translate(matrix, translation);
    return matrix;
}