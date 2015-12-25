#ifndef _DOUBLE_TO_INT_SEQUENCE_H
#define _DOUBLE_TO_INT_SEQUENCE_H

#include <array>
#include <cassert>
#include <cmath>

/**
 * @brief zerlegt Double-Zahlen in eine Reihe von Integern, die bestmöglich die Double-Zahl im Mittel approximieren.
 *
 * z.&nbsp;B.:
 * 3,5 = { 4, 3, 4, 3, 4, 3 }, entspricht genau 3,5
 * 3,2478 = { 3, 3, 4, 3, 3, 3 }, entspricht 3,166666667
 *
 * Der Algorithmus ist wie folgt:
 * - Sei n die Länge der Zielreihe und x die Double-Zahl.
 * - Es wird X=x*n berechnet und das Ergebnis gerundet.
 * - Die Division X/n ergibt nun einen Bruch, der mit n-teln ausgedrückt werden kann.
 *   D.&nbsp;h. X/n = Y + y/n, wobei Y und y Ganzzahlen sind.
 * - Die Reihe ergibt sich aus (n-y) Zahlen Y und y Zahlen (Y+1).
 */
class DoubleToIntSequence {

public:
    static std::array<int, 6> toIntSequence(double value) {
        assert (value >= 0); // Negative Zahlen brauchen wir nicht ;-)

        // Zahl durch 6 teilbar machen
        int intValue = int(std::round(value * 6));

        // Dividieren
        int wholeNumber = intValue / 6;
        int remainder = intValue % 6;

        // Je nach Rest nun das Ergebnis aufbauen. Dabei werden die verschiedenen Zahlen bestmöglichst aufgeteilt.
        std::array<int, 6> result;

        if (remainder == 0) {
            result[0] = wholeNumber;
            result[1] = wholeNumber;
            result[2] = wholeNumber;
            result[3] = wholeNumber;
            result[4] = wholeNumber;
            result[5] = wholeNumber;
        }
        else if (remainder == 1) {
            result[0] = wholeNumber;
            result[1] = wholeNumber;
            result[2] = wholeNumber;
            result[3] = wholeNumber + 1;
            result[4] = wholeNumber;
            result[5] = wholeNumber;
        }
        else if (remainder == 2) {
            result[0] = wholeNumber;
            result[1] = wholeNumber;
            result[2] = wholeNumber + 1;
            result[3] = wholeNumber;
            result[4] = wholeNumber;
            result[5] = wholeNumber + 1;
        }
        else if (remainder == 3) {
            result[0] = wholeNumber;
            result[1] = wholeNumber + 1;
            result[2] = wholeNumber;
            result[3] = wholeNumber + 1;
            result[4] = wholeNumber;
            result[5] = wholeNumber + 1;
        }
        else if (remainder == 4) {
            result[0] = wholeNumber + 1;
            result[1] = wholeNumber + 1;
            result[2] = wholeNumber;
            result[3] = wholeNumber + 1;
            result[4] = wholeNumber + 1;
            result[5] = wholeNumber;
        }
        else if (remainder == 5) {
            result[0] = wholeNumber + 1;
            result[1] = wholeNumber + 1;
            result[2] = wholeNumber + 1;
            result[3] = wholeNumber;
            result[4] = wholeNumber + 1;
            result[5] = wholeNumber + 1;
        }
        else {
            assert(false); // Es können nur Reste 0 bis 5 auftreten.
        }

        return result;
    }

};

#endif