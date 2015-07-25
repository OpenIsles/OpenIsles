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

std::string toString(long x) {
    return std::to_string(x);
}

std::string toString(double x) {
    return std::to_string(x);
}

int stringToInteger(std::string s, int base) {
    return std::stoi(s, 0, base);
}

unsigned long stringToUnsignedLong(std::string s) {
    return std::stoul(s);
}

double stringToDouble(std::string s) {
    return std::stod(s);
}
#endif

#ifdef WINDOWS
#include <cassert>
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

std::string toString(long x) {
    char buffer[20];
    sprintf(buffer, "%ld", x);
    return std::string(buffer);
}

std::string toString(double x) {
    char buffer[30];
    sprintf(buffer, "%f", x);
    return std::string(buffer);
}

int stringToInteger(std::string s, int base) {
    assert(base == 10); // TODO unter Windows ausprobieren, wie man eine andere Basis hinkriegt

    int x;
    sscanf(s.c_str(), "%d", &x);
    return x;
}

unsigned long stringToUnsignedLong(std::string s) {
    unsigned long x;
    sscanf(s.c_str(), "%lu", &x);
    return x;
}

double stringToDouble(std::string s) {
    double x;
    sscanf(s.c_str(), "%lf", &x);
    return x;
}

#endif