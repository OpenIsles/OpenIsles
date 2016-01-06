#ifndef _STRING_FORMAT_H
#define _STRING_FORMAT_H

#include <string>

/**
 * @brief Analog std::sprintf(), allerdings mit std::string-Input und -Output.
 * @param formatString Format-Zeichenkette (siehe std::printf())
 * @param vlist va_list mit den Parametern
 * @return String
 *
 * Wichtig: Parameter für '%s' müssen trotzdem C-Strings (char*) sein!
 * Bei Übergabe eines std::string kommt Speichermist raus!
 */
std::string string_sprintf_va_list(const std::string& formatString, va_list vlist);

/**
 * @brief Analog std::sprintf(), allerdings mit std::string-Input und -Output.
 * @param formatString Format-Zeichenkette (siehe std::printf())
 * @param ... Parameter
 * @return String
 *
 * Wichtig: Parameter für '%s' müssen trotzdem C-Strings (char*) sein!
 * Bei Übergabe eines std::string kommt Speichermist raus!
 */
std::string string_sprintf(const std::string& formatString, ...);

/**
 * Wrapper, um eine Zahl zu einem String umzuwandeln.
 *
 * @param x Zahl
 * @return String
 */
std::string toString(unsigned int x);

/**
 * Wrapper, um eine Zahl zu einem String umzuwandeln.
 *
 * @param x Zahl
 * @return String
 */
std::string toString(int x);

/**
 * Wrapper, um eine Zahl zu einem String umzuwandeln.
 *
 * @param x Zahl
 * @return String
 */
std::string toString(unsigned long x);

/**
 * Wrapper, um eine Zahl zu einem String umzuwandeln.
 *
 * @param x Zahl
 * @return String
 */
std::string toString(long x);

/**
 * Wrapper, um eine Zahl zu einem String umzuwandeln.
 *
 * @param x Zahl
 * @return String
 */
std::string toString(double x);

/**
 * Wrapper, um einen String in eine Zahl umzuwandeln.
 *
 * @param s String
 * @param base Zahlenbasis
 * @return Zahl
 */
int stringToInteger(std::string s, int base = 10);

/**
 * Wrapper, um einen String in eine Zahl umzuwandeln.
 *
 * @param s String
 * @return Zahl
 */
unsigned long stringToUnsignedLong(std::string s);

/**
 * Wrapper, um einen String in eine Zahl umzuwandeln.
 *
 * @param s String
 * @return Zahl
 */
double stringToDouble(std::string s);

#endif