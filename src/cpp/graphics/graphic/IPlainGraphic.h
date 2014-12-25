#ifndef _I_PLAIN_GRAPHIC_H
#define _I_PLAIN_GRAPHIC_H

#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Interface für eine einfache Grafik
 */
class IPlainGraphic : public virtual IGraphic {

public:
    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
    virtual ~IPlainGraphic() {}

    /**
     * @brief Zeichnet die Grafik
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     */
    virtual void drawAt(int x, int y) = 0;

    /**
     * @brief Zeichnet die Grafik skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     */
    virtual void drawScaledAt(int x, int y, double scale) = 0;
};

#endif