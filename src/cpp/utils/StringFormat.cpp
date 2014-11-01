#include "utils/StringFormat.h"

#ifdef LINUX
std::string toString(unsigned int x) {
    return std::to_string(x);
}
#endif

#ifdef WINDOWS
#include <cstdio>

std::string toString(unsigned int x) {
    char buffer[20];
    sprintf(buffer, "%u", x);
    return std::string(buffer);
}
#endif