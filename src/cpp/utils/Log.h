#ifndef _LOG_H
#define _LOG_H

#include <cstdarg>
#include <iostream>
#include <string>
#include "utils/StringFormat.h"


/**
 * @brief Klasse mit statischen Methoden zum Loggen
 */
class Log {

public:
    /**
     * @brief Loggt einen Fehler auf stderr.
     * @param formatString Format-Zeichenkette (siehe std::printf())
     * @param vlist va_list mit den Parametern
     */
    static void error(const std::string& formatString, ...) {
        va_list vlist;
        va_start(vlist, formatString);
        std::cerr << "[ERROR] " << string_sprintf_va_list(formatString, vlist) << std::endl;
        va_end(vlist);
    }

    /**
     * @brief Loggt eine Information auf stdout.
     * @param formatString Format-Zeichenkette (siehe std::printf())
     * @param vlist va_list mit den Parametern
     */
    static void info(const std::string& formatString, ...) {
        va_list vlist;
        va_start(vlist, formatString);
        std::cout << "[ INFO] " << string_sprintf_va_list(formatString, vlist) << std::endl;
        va_end(vlist);
    }

    /**
     * @brief Loggt einen Debug-Text auf stderr.
     * @param formatString Format-Zeichenkette (siehe std::printf())
     * @param vlist va_list mit den Parametern
     */
    static void debug(const std::string& formatString, ...) {
        va_list vlist;
        va_start(vlist, formatString);
        std::cerr << "[DEBUG] " << string_sprintf_va_list(formatString, vlist) << std::endl;
        va_end(vlist);
    }

};

#endif