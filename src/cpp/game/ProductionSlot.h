#ifndef _PRODUCTION_SLOT_H
#define _PRODUCTION_SLOT_H

/**
 * @brief Einfacher Bitmask-Enum, der angibt, welcher Produktions-Slots gemeint ist.
 * @see ProductionSlots
 */
typedef
enum ProductionSlot : unsigned char {

    /**
     * @brief produzierte Güter
     * @see ProductionSlots#output
     */
    OUTPUT = 1,

    /**
     * @brief verbrauchte Güter
     * @see ProductionSlots#input
     */
    INPUT = 2,

    /**
     * @brief weitere verbrauchte Güter, wenn das Gebäude aus zwei verschiedenen Gütern was herstellt
     * @see ProductionSlots#input2
     */
    INPUT2 = 4

} ProductionSlot;

#endif