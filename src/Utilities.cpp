#include "Utilities.hpp"

using namespace std;

long Utilities::getMicrotime()
{
    struct timespec now;
    clock_gettime( CLOCK_MONOTONIC_RAW, &now );
    return (uint64_t)now.tv_sec * 1000000000U + (uint64_t)now.tv_nsec;
}