#ifndef UTILS_H
#define UTILS_H

#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace Utils {
    template <class T> void CheckIfVectorOutOfMemory(std::vector<T> *vec, int expectedSize);
    glm::mat4 MatrixFromPosRot(float position[3], float euler[3]);
    void CheckForError(bool assertion, std::string format, ...);
    void TryAndCatchError(bool assertion, std::string format, ...);
    void CheckForWarning(bool assertion, std::string format, ...);
    void CheckForWarning(bool *warningOccured, bool assertion, std::string format, ...);
    void TryAndCatchWarning(bool *warningOccured, bool assertion, std::string format, ...);
}

#endif