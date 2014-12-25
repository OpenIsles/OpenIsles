#ifndef _I_GRAPHIC_H
#define _I_GRAPHIC_H

#include <cstdint>
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Interface für die Basisklasse für alle Grafiken. Kann nix außer die Grafik zu laden und entladen
 */
class IGraphic {

public:
    virtual ~IGraphic() {}

    /**
     * @brief Liefert die Breite der Grafik zurück
     * @return Breite der Grafik
     */
    virtual int getWidth() const = 0;

    /**
     * @brief Liefert die Höhe der Grafik zurück
     * @return Höhe der Grafik
     */
    virtual int getHeight() const = 0;

    /**
     * @brief Liest die Farbwerte eines bestimmten Pixels aus und setzt sie in die r-, g-, b- und a-Parameter.
     * @param x x-Koordinate (IN)
     * @param y y-Koordinate (IN)
     * @param r wird auf den Rot-Wert des Pixels gesetzt (OUT)
     * @param g wird auf den Grün-Wert des Pixels gesetzt (OUT)
     * @param b wird auf den Blau-Wert des Pixels gesetzt (OUT)
     * @param a wird auf den Alpha-Wert des Pixels gesetzt (OUT) 0 = voll transparent, 255 = absolut undurchsichtig
     */
    virtual void getPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) = 0;

};

#endif
