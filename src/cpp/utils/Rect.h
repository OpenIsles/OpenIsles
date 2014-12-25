#ifndef _RECT_H
#define	_RECT_H

/**
 * @brief Hilfsklasse, die ein Rechteck darstellt
 */
struct Rect {
    /**
     * Start-X-Koordinate
     */
    int x;

    /**
     * Start-Y-Koordinate
     */
    int y;

    /**
     * Breite
     */
    int w;

    /**
     * Höhe
     */
    int h;


    /**
     * @brief Konstruiert ein neues Rechteck mit 0-Werten
     */
    Rect() : x(0), y(0), w(0), h(0) {
    }

    /**
     * @brief Konstruiert ein neues Rechteck.
     * @param x Start-X-Koordinate
     * @param y Start-Y-Koordinate
     * @param w Breite
     * @param h Höhe
     */
    Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {
    }

    /**
     * @brief Kopierkonstruktur.
     * @param rect Rechteck, was als Vorlage benutzt werden soll
     */
    Rect(const Rect& rect) : x(rect.x), y(rect.y), w(rect.w), h(rect.h) {
    }

};

#endif