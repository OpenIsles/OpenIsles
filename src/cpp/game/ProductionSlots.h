#ifndef _PRODUCTION_SLOTS_H
#define _PRODUCTION_SLOTS_H

#include "game/GoodsSlot.h"

/**
 * @brief Enthält die Slots für produzierte und zu verbrauchende Güter eines Gebäudes.
 */
typedef
struct ProductionSlots {

    /**
     * @brief produzierte Güter
     */
    GoodsSlot output;

    /**
     * @brief verbrauchte Güter
     */
    GoodsSlot input;

    /**
     * @brief weitere verbrauchte Güter, wenn das Gebäude aus zwei verschiedenen Gütern was herstellt
     */
    GoodsSlot input2;

} ProductionSlots;


#endif