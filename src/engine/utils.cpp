#include "engine/utils.hpp"
#include "game/scene.hpp"

#include "glm/gtx/euler_angles.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

void vCheckError(bool assertion, std::string format, va_list args)
{
    if (!assertion)
    {
        vfprintf(stderr, format.c_str(), args);
        fprintf(stderr, "\n");
        abort();
    }
}

void Utils::CheckForError(bool assertion, std::string format, ...)
{
    va_list args;
    va_start(args, format);
    vCheckError(assertion, format, args);
}

// Exactly the same as above. Just for code clarity.
void Utils::TryAndCatchError(bool assertion, std::string format, ...)
{
    va_list args;
    va_start(args, format);
    vCheckError(assertion, format, args);
}

void vCheckWarning(bool *warningOccured, bool assertion, std::string format, va_list args)
{
    if (!assertion)
    {
        vfprintf(stderr, format.c_str(), args);
        fprintf(stderr, "\n");
        *warningOccured = true; // DON'T set to false otherwise; allows easy stacking of warning checks.
    }
}

void Utils::CheckForWarning(bool assertion, std::string format, ...)
{
    va_list args;
    va_start(args, format);
    bool unused;
    vCheckWarning(&unused, assertion, format, args);
}

void Utils::CheckForWarning(bool *warningOccured, bool assertion, std::string format, ...)
{
    va_list args;
    va_start(args, format);
    vCheckWarning(warningOccured, assertion, format, args);
}

// Exactly the same as above. Just for code clarity.
void Utils::TryAndCatchWarning(bool *warningOccured, bool assertion, std::string format, ...)
{
    va_list args;
    va_start(args, format);
    vCheckWarning(warningOccured, assertion, format, args);
}