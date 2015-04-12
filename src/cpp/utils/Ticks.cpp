#include "Ticks.h"

#ifndef NO_SDL
#include <SDL_timer.h>

uint32_t getTicks() {
    return SDL_GetTicks();
}

#else

/**
 * @brief Zeitstempel, den wir grade als aktuell vorgaukeln
 */
static uint32_t fakeTime = 0;

uint32_t getTicks() {
    return fakeTime;
}

void setTicks(uint32_t ticks) {
    fakeTime = ticks;
}

#endif