#ifndef _DIRECTIONS_H
#define _DIRECTIONS_H

#include <stdexcept>
#include <string>


/**
 * @page direction Ansichten/Richtungen
 *
 * In der Anwendung gibt es verschiedene Ansichten/Richtungen, z.&nbsp;B.:
 * <ul>
 *     <li>Aus welcher Sicht sehen wir die Karte?</li>
 *     <li>Welche Ansicht des Gebäude ist wohin gedreht?</li>
 *     <li>In welche Richtung läuft der Träger bzw. fährt das Schiff?</li>
 * </ul>
 *
 * <p>
 *     Um diese einfach abbilden zu können, gibt es die beiden Typen `FourthDirection` und `EighthDirection`.
 *     Effektiv sind beide auf einen `char` im Bereich 0 bis 7 abgebildet. Die geraden Werte 0, 2, 4 und 6 stellen
 *     die Haupthimmelsrichtungen dar, die ungeraden die Zwischenwinkel. Im Code ist es wichtig zu wissen, ob
 *     wir 90 Grad- oder 45 Grad-Winkel haben, deshalb gibt es diese zwei Typ-Aliase.
 * </p>
 * <pre>
 *                    4
 *                  Nord
 *     5                               3
 *       Nordwest     ↑       Nordost
 *               ↖    |    ↗
 *                 \  |  /
 *                   \|/
 *   6   West  ←‒‒‒‒‒‒O‒‒‒‒‒‒→  Ost   2
 *                   /|\
 *                 /  |  \
 *               ↙    |    ↘
 *       Südwest      ↓       Südost
 *     7                             1
 *                   Süd
 *                    0
 * </pre>
 */

/**
 * @brief Typ-Alias für die 4 Ansichten/Richtungen, die durch zwei teilbar sind, d.&nbsp;h. Süd, Ost, Nord, West
 */
using FourthDirection = char;
/**
 * @brief Typ-Alias für alle 8 möglichen Ansichten/Richtungen
 */
using EighthDirection = char;

/**
 * @brief Makro um über Süd, Ost, Nord, West zu iterieren
 */
#define forEachFourthDirection(var) for (FourthDirection var = Direction::MIN; var < Direction::MAX; var += 2)
/**
 * @brief Makro um über alle 8 möglichen Ansichten/Richtungen zu iterieren
 */
#define forEachEighthDirection(var) for (EighthDirection var = Direction::MIN; var < Direction::MAX; var++ )


/**
 * @brief Klasse, die uns die Möglichkeiten gibt, mit FourthDirection und EighthDirection zu rechnen
 */
class Direction {

public:
    /**
     * @brief Keine/ungültige Ansicht/Richtung
     */
    static constexpr char NONE = -1;

    /**
     * @brief Süd
     */
    static constexpr char SOUTH     = 0;
    /**
     * @brief Südost
     */
    static constexpr char SOUTHEAST = 1;
    /**
     * @brief Ost
     */
    static constexpr char EAST      = 2;
    /**
     * @brief Nordost
     */
    static constexpr char NORTHEAST = 3;
    /**
     * @brief Nord
     */
    static constexpr char NORTH     = 4;
    /**
     * @brief Nordwest
     */
    static constexpr char NORTHWEST = 5;
    /**
     * @brief West
     */
    static constexpr char WEST      = 6;
    /**
     * @brief Südwest
     */
    static constexpr char SOUTHWEST = 7;

    /**
     * @brief minimaler Wert
     */
    static constexpr char MIN = 0;
    /**
     * @brief größter Wert, der grade nicht mehr angenommen werden kann
     */
    static constexpr char MAX = 8;

public:
    /**
     * @brief Ermittelt eine Ansicht/Richtung aus einem String.
     * Diese Methode ist sehr teuer und sollte keinesfalls beim Rendern gebraucht werden!
     *
     * @param dirName String, der die Ansicht/Richtung benennt
     * @return EighthDirection
     */
    static EighthDirection fromString(const std::string& dirName) {
        if (dirName == "south") {
            return Direction::SOUTH;
        } else if (dirName == "southeast") {
            return Direction::SOUTHEAST;
        } else if (dirName == "east") {
            return Direction::EAST;
        } else if (dirName == "northeast") {
            return Direction::NORTHEAST;
        } else if (dirName == "north") {
            return Direction::NORTH;
        } else if (dirName == "northwest") {
            return Direction::NORTHWEST;
        } else if (dirName == "west") {
            return Direction::WEST;
        } else if (dirName == "southwest") {
            return Direction::SOUTHWEST;
        } else {
            throw std::runtime_error("Illegal dirName");
        }
    }

    /**
     * @brief Rotiert eine Ansicht/Richtung im Uhrzeigersinn um 90 Grad.
     * @param dir Referenz auf die zu drehende Ansicht/Richtung
     */
    static inline void rotate90DegreesClockwise(FourthDirection& dir) {
        rotate90DegreesClockwise(dir, 1);
    }

    /**
     * @brief Rotiert eine Ansicht/Richtung im Uhrzeigersinn um ein Vielfaches von 90 Grad.
     * @param dir Referenz auf die zu drehende Ansicht/Richtung
     * @param times gibt die Zahl der Vielfachen an
     */
    static inline void rotate90DegreesClockwise(FourthDirection& dir, int times) {
        dir += 2*times;
        if (dir >= MAX) {
            dir -= MAX;
        }
    }

    /**
     * @brief Rotiert eine Ansicht/Richtung gegen Uhrzeigersinn um 90 Grad.
     * @param dir Referenz auf die zu drehende Ansicht/Richtung
     */
    static inline void rotate90DegreesCounterclockwise(FourthDirection& dir) {
        rotate90DegreesCounterclockwise(dir, 1);
    }

    /**
     * @brief Rotiert eine Ansicht/Richtung gegen Uhrzeigersinn um ein Vielfaches von 90 Grad.
     * @param dir Referenz auf die zu drehende Ansicht/Richtung
     * @param times gibt die Zahl der Vielfachen an
     */
    static inline void rotate90DegreesCounterclockwise(FourthDirection& dir, int times) {
        dir -= 2*times;
        if (dir < MIN) {
            dir += MAX;
        }
    }

    /**
     * @brief Addiert auf eine Ansicht/Richtung einen Offset (= Vielfaches von 90 Grad) und gibt die neue
     * Ansicht/Richtung zurück.
     *
     * @param dir Referenz auf die Ansicht/Richtung, die als Basis benutzt werden soll
     * @param offset gibt den Offset (= die Zahl der Vielfachen) an
     * @return gedrehte Ansicht/Richtung
     */
    static inline EighthDirection add90DegreeOffset(const EighthDirection& dir, unsigned char offset) {
        return (dir + offset*2) % MAX;
    }

    /**
     * @brief Addiert zwei Ansichten/Richtungen.
     *
     * @param dir1 Referenz auf die Ansicht/Richtung, die als erster Summand benutzt werden soll
     * @param dir2 Referenz auf die Ansicht/Richtung, die als zweiter Summand benutzt werden soll
     * @return gedrehte Ansicht/Richtung
     */
    static inline EighthDirection addDirections(const EighthDirection& dir1, const EighthDirection& dir2) {
        return (dir1 + dir2) % MAX;
    }

};

#endif