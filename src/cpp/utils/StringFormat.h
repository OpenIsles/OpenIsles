#ifndef _STRING_FORMAT_H
#define	_STRING_FORMAT_H

#include <string>

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
std::string toString(double x);

#endif