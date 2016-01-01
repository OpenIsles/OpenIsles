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
     * @brief `true`, wenn das Gut ein Rohstoff ist.
     * Rohstoffe werden nur vom Marktkarren abgeholt, wenn das Gebäudelager voll ist.
     */
    bool rawMaterial;

    /**
     * @brief `true` für unsichtbares Gut.
     * Unsichtbare Güter werden für die Spiellogik gebraucht, haben aber kein Gütersymbol und werden dem Benutzer
     * auch nicht angezeigt.
     */
    bool invisible;


    /**
     * @brief Liefert die Bezeichnung des Guts (msgid)
     * @return Bezeichnung des Guts (msgid)
     */
    std::string getTitleMsgid() const {
        return "good|" + name;
    }
};

#endif