#ifndef _PLAIN_GRAPHIC_H
#define _PLAIN_GRAPHIC_H

#include <SDL.h>
#include <SDL_image.h>
#include "graphics/Graphic.h"

/**
 * @brief Einfache Grafik
 */
class PlainGraphic : public Graphic {

public:
    /**
     * @brief Konstruktor. Lädt die Grafik.
     * @param filename Dateiname der zu ladenden Grafik
     */
    PlainGraphic(const char* filename);

    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
    virtual ~PlainGraphic();

    /**
     * @brief Zeichnet die Grafik
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     */
    void drawAt(int x, int y);

    /**
     * @brief Zeichnet die Grafik skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     */
    void drawScaledAt(int x, int y, double scale);
};

#endif
