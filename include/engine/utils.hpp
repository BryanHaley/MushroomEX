#ifndef UTILS_H
#define UTILS_H

#include "glm/glm.hpp"

#include <vector>

namespace Utils {
    template <class T> void CheckIfVectorOutOfMemory(std::vector<T> *vec, int expectedSize);
    glm::mat4 MatrixFromPosRot(float position[3], float euler[3]);
}

#endif