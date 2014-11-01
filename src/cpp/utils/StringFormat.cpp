#include "utils/StringFormat.h"

#ifdef LINUX
std::string toString(unsigned int x) {
    return std::to_string(x);
}

std::string toString(int x) {
    return std::to_string(x);
}

std::string toString(unsigned long x) {
    return std::to_string(x);
}

std::string toString(double x) {
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

std::string toString(int x) {
    char buffer[20];
    sprintf(buffer, "%d", x);
    return std::string(buffer);
}

std::string toString(unsigned long x) {
    char buffer[20];
    sprintf(buffer, "%lu", x);
    return std::string(buffer);
}

std::string toString(double x) {
    char buffer[30];
    sprintf(buffer, "%lf", x);
    return std::string(buffer);
}
#endif