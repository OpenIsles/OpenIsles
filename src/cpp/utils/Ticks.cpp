#include "Ticks.h"

#ifndef NO_SDL
#include <SDL_timer.h>

/**
* @brief Liefert die aktuelle Zeit in Form von Ticks zurück.
* @return Ticks (= Millisekunden seit SDL-Initialisierung)
*/
uint32_t getTicks() {
    return SDL_GetTicks();
}

#else
#include <ctime>

/**
 * @brief Liefert die aktuelle Zeit in Form von Ticks zurück.
 * @return Ticks (= Millisekunden seit Unix-Null)
 */
uint32_t getTicks() {
    return time(nullptr);
}

#endif