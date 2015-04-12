#ifndef _TICKS_H
#define _TICKS_H

#include <cstdint>

/**
 * @brief Liefert die aktuelle Zeit in Form von Ticks zurück.
 * @return Ticks (= Millisekunden)
 */
uint32_t getTicks();


#ifdef NO_SDL

/**
 * @brief Setzt die aktuelle Zeit in Form von Ticks. getTicks() wird diese dann zurückliefern.
 * @param ticks Ticks (= Millisekunden seit Unix-Null)
 */
void setTicks(uint32_t ticks);

#endif

#endif
