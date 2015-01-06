#ifndef _DIRECTIONS_H
#define _DIRECTIONS_H

#include <string>

/**
 * @brief Repräsentiert eine mögliche von 4 Ansichten (Süd, Ost, Nord, West). Damit können wir abstrakt die Ansicht
 * benennen und haben Array-Index (0-3) oder GraphicSetKeyView an einer Stelle.
 *
 * Intern wird zugeordnet
 * - SOUTH = 0
 * - EAST = 1
 * - NORTH = 2
 * - WEST = 3
 */
class FourDirectionsView {

private:
    /**
     * @brief Namen der Views
     */
    static const std::string viewNames[];

private:
    /**
     * @brief Index im Bereich 0 bis 3 für Array-Zugriff. Wir benutzen einen signed-Typ, damit wir im operator--
     * kurzfristig die -1 haben können.
     */
    char viewIndex;

public:
    /**
     * @brief Konstruktor. Stellt die Ansicht auf "Süd" ein.
     */
    FourDirectionsView() {
        viewIndex = 0;
    }

    /**
     * @brief "Inkrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um 90° im Uhrzeigersinn gedreht.
     * (Wir rotieren gegen den Uhrzeigersinn.)
     */
    FourDirectionsView operator++(int) {
        FourDirectionsView view = *this;

        if (++viewIndex == 4) {
            viewIndex = 0;
        }

        return view;
    }

    /**
     * @brief "Dekrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um 90° gegen Uhrzeigersinn gedreht.
     * (Wir rotieren im Uhrzeigersinn.)
     */
    FourDirectionsView operator--(int) {
        FourDirectionsView view = *this;

        if (--viewIndex == -1) {
            viewIndex = 3;
        }

        return view;
    }

    /**
     * @brief Liefert einen Array-Index für die Ansicht zurück.
     * @return Array-Index im Bereich 0 bis 3
     */
    unsigned char getViewIndex() const {
        return (unsigned char) viewIndex;
    }

    /**
     * @brief Liefert einen String mit dem Namen der Ansicht zurück
     * @return "north", "east", "south" oder "west"
     */
    const std::string& getViewName() const {
        return viewNames[(int) viewIndex];
    }
};


#endif