#ifndef _I_GRAPHIC_H
#define _I_GRAPHIC_H

#include <cstdint>
#include "graphics/renderer/IRenderer.h"

/**
 * @brief Interface für eine Einzelgrafik
 */
class IGraphic {

public:
    /**
     * @brief zeichnet das Objekt maskiert, d.h. gelbem Overlay für Gebäudeplatzierung
     * @sa #draw()
     */
    static const int DRAWING_FLAG_MASKED   = (1 << 0);

    /**
     * @brief zeichnet das Objekt rot eingefärbt
     * Info: Rot und abgedunkelt schließen sich gegenseitig aus.
     * @sa #draw()
     */
    static const int DRAWING_FLAG_RED      = (1 << 1);

    /**
     * @brief zeichnet das Objekt blinkend, d.h. entweder wird die Grafik gezeichnet oder nicht
     * @sa #draw()
     */
    static const int DRAWING_FLAG_BLINK    = (1 << 2);

    /**
     * @brief zeichnet das Objekt abgedunkelt
     * Info: Rot und abgedunkelt schließen sich gegenseitig aus.
     * @sa #draw()
     */
    static const int DRAWING_FLAG_DARKENED = (1 << 3);

    /**
     * @brief zeichnet das Objekt als grauen Schatten (kann nicht mit `DRAWING_FLAG_MASKED` kombiniert werden)
     * @sa #draw()
     */
    static const int DRAWING_FLAG_SHADOW   = (1 << 4);

protected:
    /**
	 * @brief Breite der Grafik
	 */
	int width;

	/**
	 * @brief Höhe der Grafik
	 */
	int height;

public:
    virtual ~IGraphic() {}

    /**
     * @brief Liefert die Breite der Grafik zurück
     * @return Breite der Grafik in Pixel
     */
    int getWidth() const {
        return width;
    }

    /**
     * @brief Liefert die Höhe der Grafik zurück
     * @return Höhe der Grafik in Pixel
     */
    int getHeight() const {
        return height;
    }

    /**
     * @brief Liest die Farbwerte eines bestimmten Pixels aus und setzt sie in die r-, g-, b- und a-Parameter.
     * @param x x-Koordinate (IN)
     * @param y y-Koordinate (IN)
     * @param r wird auf den Rot-Wert des Pixels gesetzt (OUT)
     * @param g wird auf den Grün-Wert des Pixels gesetzt (OUT)
     * @param b wird auf den Blau-Wert des Pixels gesetzt (OUT)
     * @param a wird auf den Alpha-Wert des Pixels gesetzt (OUT) 0 = voll transparent, 255 = absolut undurchsichtig
     */
    virtual void getPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) const = 0;

    /**
     * @brief Zeichnet die Grafik
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     */
    virtual void drawAt(int x, int y) const = 0;

    /**
     * @brief Zeichnet die Grafik skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     */
    virtual void drawScaledAt(int x, int y, double scale) const = 0;

    /**
     * @brief Zeichnet die Schatten-Grafik skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     */
    virtual void drawShadowScaledAt(int x, int y, double scale) const = 0;

     /**
     * @brief Zeichnet die Grafik mit bestimmten Drawing-Flags. Quell- und Zielbereich können beliebig gewählt werden.
     * @param rectSource Quell-Rechteck der Zeichenoperation oder nullptr, wenn die komplette Grafik verwendet werden soll
     * @param rectDestination Ziel-Rechteck der Zeichenoperation oder nullptr, wenn ganzflächig gemalt werden soll
     * @param drawingFlags Bitmaske aus DRAWING_FLAG_*-Konstanten, die angeben, wie die Grafik gezeichnet werden soll
     * @param sdlTicks aktuelle Ticks aus `Context`, um den Blinke-Zustand zu kennen
     */
    virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) const = 0;

};

#endif
