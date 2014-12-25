#ifndef _SDL_PLAIN_GRAPHIC_H
#define _SDL_PLAIN_GRAPHIC_H

#include "graphics/graphic/IPlainGraphic.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Einfache Grafik
 */
class SDLPlainGraphic : public SDLGraphic, public virtual IPlainGraphic {

public:
    /**
     * @brief Konstruktor. Lädt die Grafik.
     * @param renderer (Dependency)
     * @param filename Dateiname der zu ladenden Grafik
     */
    SDLPlainGraphic(IRenderer* const renderer, const char* filename);

    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
    virtual ~SDLPlainGraphic();

    /**
     * @brief Zeichnet die Grafik
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     */
    virtual void drawAt(int x, int y);

    /**
     * @brief Zeichnet die Grafik skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     */
    virtual void drawScaledAt(int x, int y, double scale);
};

#endif
