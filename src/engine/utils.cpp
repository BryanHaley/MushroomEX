#include "engine/utils.hpp"
#include "game/scene.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace Utils;

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