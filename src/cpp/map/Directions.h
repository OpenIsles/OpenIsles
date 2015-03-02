#ifndef _DIRECTIONS_H
#define _DIRECTIONS_H

#include <stdexcept>
#include <string>


template <unsigned char N>
class NDirectionsView {

public:
    /**
     * @brief viewIndex, der den ungültigen Zustand darstellt.
     */
    static constexpr char INVALID  = -1;

    /**
     * @brief Minimaler viewIndex (inklusive). Muss 0 sein, sonst klappt unser Ring-Rechnen nicht.
     */
    static constexpr char MIN_VIEW = 0;

    /**
     * @brief Maximaler viewIndex (exklusive)
     */
    static constexpr char MAX_VIEW = N;

protected:
    /**
     * @brief Index im Bereich MIN_VIEW bis (MAX_VIEW-1) für Array-Zugriff oder -1.
     *
     * Wir benutzen einen signed-Typ, damit wir
     *  a) kurzfristig während einer Berechnug negative Werte haben können und
     *  b) einen Invalid-Wert haben
     */
    char viewIndex;

public:
    /**
     * @brief Default-Konstruktor. Erzeugt eine ungültige Ansicht.
     * Die Unterklasse benutzt diesen Default-Konstruktor vor dem eigentlichen Setzen von viewIndex.
     */
    NDirectionsView() : viewIndex(INVALID) {
    }

    /**
     * @brief Konstruktor, der eine bestimmte View erstellt.
     * Ist der Konstruktions-Parameter nicht im Bereich MIN_VIEW bis (MAX_VIEW-1), so wird durch Ring-Rechnen
     * der Wert korrigiert.
     *
     * @param viewIndex zu setzender View-Index
     */
    NDirectionsView(char viewIndex) {
        this->viewIndex = viewIndex % N;
        if (this->viewIndex < 0) {
            this->viewIndex += N;
        }
    }

    /**
     * @brief Liefert einen Array-Index für die Ansicht zurück.
     * @return Array-Index im Bereich MIN_VIEW bis (MAX_VIEW-1) oder INVALID.
     */
    char getViewIndex() const {
        return viewIndex;
    }

protected:
    /**
     * @brief Stellt sicher, dass nicht der ungültige Wert vorliegt. Falls doch, wirft diese Methode eine Exception.
     */
    void assertNotInvalid(void) const {
        if (viewIndex == INVALID) {
            throw std::runtime_error("With INVALID cannot be calculated");
        }
    }
};


/**
 * @brief Repräsentiert eine mögliche von 8 Ansichten (Süd, Südost, Ost, Nordost, Nord, Nordwest, West, Südwest).
 */
class EightDirectionsView : public NDirectionsView<8> {

public:
    static constexpr char SOUTH     = 0;
    static constexpr char SOUTHEAST = 1;
    static constexpr char EAST      = 2;
    static constexpr char NORTHEAST = 3;
    static constexpr char NORTH     = 4;
    static constexpr char NORTHWEST = 5;
    static constexpr char WEST      = 6;
    static constexpr char SOUTHWEST = 7;

    /**
     * @brief Enthält alle Ansichten im Array zur Verwendung in foreach-Loops
     */
    static const EightDirectionsView ALL_VIEWS[8];


    /**
     * @brief Default-Konstruktor. Erzeugt eine ungültige Ansicht.
     */
    explicit EightDirectionsView() : NDirectionsView() {
    }

    /**
     * @brief Konstruktor, der eine bestimmte View erstellt.
     * Ist der Konstruktions-Parameter nicht im Bereich MIN_VIEW bis (MAX_VIEW-1), so wird durch Ring-Rechnen
     * der Wert korrigiert.
     *
     * @param viewIndex zu setzender View-Index
     */
    explicit EightDirectionsView(char viewIndex) : NDirectionsView(viewIndex) {
    }

    /**
     * @brief Konstruktor, der eine bestimmte View aus einem String erstellt.
     * Dieser Konstruktur ist sehr teuer und sollte so selten wie möglich eingesetzt werden.
     *
     * @param viewName View als String
     */
    explicit EightDirectionsView(const std::string& viewName) : NDirectionsView(INVALID) {
        if (viewName == "south") {
            viewIndex = SOUTH;
        } else if (viewName == "southeast") {
            viewIndex = SOUTHEAST;
        } else if (viewName == "east") {
            viewIndex = EAST;
        } else if (viewName == "northeast") {
            viewIndex = NORTHEAST;
        } else if (viewName == "north") {
            viewIndex = NORTH;
        } else if (viewName == "northwest") {
            viewIndex = NORTHWEST;
        } else if (viewName == "west") {
            viewIndex = WEST;
        } else if (viewName == "southwest") {
            viewIndex = SOUTHWEST;
        } else {
            throw std::runtime_error("Illegal viewName");
        }
    }

    /**
     * @brief Vergleichsoperator (mit einer anderen Ansicht)
     */
    bool operator==(const EightDirectionsView& otherView) const {
        return (otherView.viewIndex == viewIndex);
    }

    /**
     * @brief Vergleichsoperator (mit Integer-ViewIndex)
     */
    bool operator==(const char& x) const {
        return (viewIndex == x);
    }

};


/**
 * @brief Repräsentiert eine mögliche von 4 Ansichten (Süd, Ost, Nord, West).
 */
class FourDirectionsView : public NDirectionsView<4> {

public:
    static constexpr char SOUTH = 0;
    static constexpr char EAST  = 1;
    static constexpr char NORTH = 2;
    static constexpr char WEST  = 3;

    /**
     * @brief Enthält alle Ansichten im Array zur Verwendung in foreach-Loops
     */
    static const FourDirectionsView ALL_VIEWS[4];

public:
    /**
     * @brief Default-Konstruktor. Erzeugt eine ungültige Ansicht.
     */
    FourDirectionsView() : NDirectionsView() {
    }

    /**
     * @brief Konstruktor, der eine bestimmte View erstellt.
     * Ist der Konstruktions-Parameter nicht im Bereich MIN_VIEW bis (MAX_VIEW-1), so wird durch Ring-Rechnen
     * der Wert korrigiert.
     *
     * @param viewIndex zu setzender View-Index
     */
    explicit FourDirectionsView(char viewIndex) : NDirectionsView(viewIndex) {
    }

    /**
     * @brief Konstruktor, der eine bestimmte View aus einem String erstellt.
     * Dieser Konstruktur ist sehr teuer und sollte so selten wie möglich eingesetzt werden.
     *
     * @param viewName View als String
     */
    explicit FourDirectionsView(const std::string& viewName) : NDirectionsView(INVALID) {
        if (viewName == "south") {
            viewIndex = SOUTH;
        } else if (viewName == "east") {
            viewIndex = EAST;
        } else if (viewName == "north") {
            viewIndex = NORTH;
        } else if (viewName == "west") {
            viewIndex = WEST;
        } else {
            throw std::runtime_error("Illegal viewName");
        }
    }

    /**
     * @brief Post-Inkrement-Operator.
     * "Inkrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um 90° im Uhrzeigersinn gedreht.
     * (Wir rotieren gegen den Uhrzeigersinn.)
     */
    FourDirectionsView operator++(int) {
        assertNotInvalid();

        FourDirectionsView view = *this;

        if (++viewIndex == MAX_VIEW) {
            viewIndex = MIN_VIEW;
        }

        return view;
    }

    /**
     * @brief Post-Dekrement-Operator.
     * "Dekrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um 90° gegen Uhrzeigersinn gedreht.
     * (Wir rotieren im Uhrzeigersinn.)
     */
    FourDirectionsView operator--(int) {
        assertNotInvalid();

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
    FourDirectionsView operator+(unsigned int x) const {
        assertNotInvalid();

        return FourDirectionsView((viewIndex + x) % MAX_VIEW);
    }

    /**
     * @brief "Inkrementiert" die Ansicht. Effektiv wird das Objekt unter der Ansicht um x*90° im Uhrzeigersinn gedreht.
     * (Wir rotieren gegen den Uhrzeigersinn.)
     */
    FourDirectionsView operator+(const FourDirectionsView& x) const {
        assertNotInvalid();
        x.assertNotInvalid();

        return FourDirectionsView(viewIndex + x.viewIndex);
    }

    /**
     * @brief Vergleichsoperator
     */
    bool operator==(const FourDirectionsView& otherView) const {
        return (otherView.viewIndex == viewIndex);
    }

    /**
     * @brief Vergleichsoperator mit Integer
     */
    bool operator==(const char& x) const {
        return (viewIndex == x);
    }

    /**
     * @brief Cast-Operator, der es erlaubt, eine FourDirectionsView in eine EightDirectionsView umzuwandeln.
     */
    operator EightDirectionsView() const {
        return EightDirectionsView(viewIndex * 2);
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

}


#endif