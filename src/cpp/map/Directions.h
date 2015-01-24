#ifndef _DIRECTIONS_H
#define _DIRECTIONS_H

#include <stdexcept>
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

public:
    /**
     * @brief Minimaler view-Index (inklusive). Muss 0 sein, sonst klappt unser Ring-Rechnen nicht.
     */
    static const int MIN_VIEW = 0;

    /**
     * @brief Maximaler view-Index (exklusive)
     */
    static const int MAX_VIEW = 4;

    /**
     * @brief Enthält alle 4 Ansichten im Array zur Verwendung in foreach-Loops
     */
    static const FourDirectionsView ALL_VIEWS[MAX_VIEW];

private:
    /**
     * @brief Namen der Views
     */
    static const std::string viewNames[];

private:
    /**
     * @brief Index im Bereich MIN_VIEW bis (MAX_VIEW-1) für Array-Zugriff. Wir benutzen einen signed-Typ, damit wir
     * im operator-- kurzfristig die -1 haben können.
     */
    char viewIndex;

public:
    /**
     * @brief Konstruktor. Stellt die Ansicht auf "Süd" ein.
     */
    FourDirectionsView() {
        viewIndex = MIN_VIEW;
    }

    /**
     * @brief Konstruktor, der das Objekt eine bestimmte Ansicht einstellt.
     * @param viewName "north", "east", "south" oder "west"
     */
    FourDirectionsView(const char* viewName) {
        for (int i = MIN_VIEW; i < MAX_VIEW; i++) {
            if (viewNames[i] == viewName) {
                viewIndex = (char) i;
                return;
            }
        }

        throw new std::runtime_error("Illegal viewName");
    }

    /**
     * @brief "Inkrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um 90° im Uhrzeigersinn gedreht.
     * (Wir rotieren gegen den Uhrzeigersinn.)
     */
    FourDirectionsView operator++(int) {
        FourDirectionsView view = *this;

        if (++viewIndex == MAX_VIEW) {
            viewIndex = MIN_VIEW;
        }

        return view;
    }

    /**
     * @brief "Dekrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um 90° gegen Uhrzeigersinn gedreht.
     * (Wir rotieren im Uhrzeigersinn.)
     */
    FourDirectionsView operator--(int) {
        FourDirectionsView view = *this;

        if (--viewIndex == MIN_VIEW - 1) {
            viewIndex = MAX_VIEW - 1;
        }

        return view;
    }

    /**
     * @brief "Inkrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um x*90° im Uhrzeigersinn gedreht.
     * (Wir rotieren gegen den Uhrzeigersinn.)
     */
    FourDirectionsView operator+(int x) {
        unsigned char newViewIndex = (this->viewIndex + x) % MAX_VIEW;

        FourDirectionsView view;
        view.viewIndex = newViewIndex;
        return view;
    }

    /**
     * @brief Liefert einen Array-Index für die Ansicht zurück.
     * @return Array-Index im Bereich MIN_VIEW bis (MAX_VIEW-1)
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


namespace std {

    /**
     * @brief std-Template-Spezialisierung, um Hash-Funktion für FourDirectionsView zu haben
     */
    template<>
    class hash<FourDirectionsView> {

    public:
        size_t operator()(const FourDirectionsView& a) const {
            return (size_t) a.getViewIndex();
        }
    };

    /**
     * @brief std-Template-Spezialisierung, um Gleichheits-Relation für FourDirectionsView zu haben
     */
    template<>
    class equal_to<FourDirectionsView> {

    public:
        bool operator()(const FourDirectionsView& a, const FourDirectionsView& b) const {
            return (a.getViewIndex() == b.getViewIndex());
        }
    };

}


#endif