#ifndef _GOOD_H
#define _GOOD_H

#include <string>

/**
 * @brief Ein einzelnes Gut
 */
struct Good {

public:
    /**
     * @brief Name des Guts, womit es die Engine identifizieren kann
     */
    std::string name;

    /**
     * @brief Bezeichnung des Guts
     */
    std::string label;

    /**
     * `true`, wenn das Gut ein Rohstoff ist.
     * Rohstoffe werden nur vom Marktkarren abgeholt, wenn das Gebäudelager voll ist.
     */
    bool rawMaterial;
};

#endif