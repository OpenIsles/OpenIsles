#ifndef _ERROR_IN_CONFIG_EXCEPTION_H
#define _ERROR_IN_CONFIG_EXCEPTION_H

#include <stdexcept>
#include "defines.h"


/**
 * @brief Exception-Klasse, die ein Problem beim Laden der Konfiguration anzeigt
 */
class ErrorInConfigException : public std::runtime_error {

public:
    /*
     * @brief (Konstruktor)
     * @param errorMessage Fehlermeldung für den Benutzer
     */
    ErrorInConfigException(const std::string& errorMessage) : std::runtime_error(errorMessage) {
    }

};

#endif
