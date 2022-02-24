#include "../../include/Utilities.hpp"

using namespace std;

uint64_t Utilities::getMicrotime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    return (uint64_t)now.tv_sec * 1000000000U + (uint64_t)now.tv_nsec;
}

//TODO: Add unit test
void Utilities::returnStringBetweenChars(string& original_string, string& new_string, string& delimiter)
{
    unsigned first = original_string.find_first_of(delimiter);
    unsigned last = original_string.find_last_of(delimiter);
    new_string = original_string.substr(first+1, last-first-1);
}

bool Utilities::String2Bool(std::string var)
{
    if(var == "true" || var == "TRUE")
        return true;
    else if(var == "false" || var == "FALSE")
        return false;
}