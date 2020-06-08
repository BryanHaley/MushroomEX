#ifndef UTILS_H
#define UTILS_H

#include <vector>

namespace Utils {
    template <class T>
    void CheckIfVectorOutOfMemory(std::vector<T> *vec, int expectedSize);
}

#endif