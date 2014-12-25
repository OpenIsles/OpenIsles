#ifndef _COLOR_H
#define	_COLOR_H

/**
 * @brief Hilfsklasse, die eine Farbe darstellt
 */
struct Color {

    /**
     * Rot-Anteil (0 bis 255)
     */
    unsigned char r;

    /**
     * Grün-Anteil (0 bis 255)
     */
    unsigned char g;

    /**
     * Blau-Anteil (0 bis 255)
     */
    unsigned char b;

    /**
     * Alphakanal (0 bis 255), 0 = transparent, 255 = opaque
     */
    unsigned char a;


    /**
     * @brief Konstruiert ein Schwarz
     */
    Color() : r(0), g(0), b(0), a(255) {
    }

    /**
     * @brief Konstruiert eine Farbe
     * @param r Rot-Anteil (0 bis 255)
     * @param g Grün-Anteil (0 bis 255)
     * @param b Blau-Anteil (0 bis 255)
     * @param a Alphakanal (0 bis 255), 0 = transparent, 255 = opaque
     */
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {
    }

    /**
     * @brief Kopierkonstruktur.
     * @param color Farbe, die als Vorlage benutzt werden soll
     */
    Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {
    }

};

#endif