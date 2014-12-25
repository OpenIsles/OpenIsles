#ifndef _I_MAP_OBJECT_GRAPHIC_H
#define _I_MAP_OBJECT_GRAPHIC_H

#include "graphics/graphic/IPlainGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "utils/Rect.h"

/**
 * @brief Interface für die Grafik eines MapObjects. Die Grafik hat eine fixe Größe in Map-Koordinaten zugeordnet,
 * sodass sie in das Kachelsystem der Map gezeichnet werden kann
 */
class IMapObjectGraphic : public virtual IPlainGraphic {

public:
    /**
     * @brief zeichnet das Objekt maskiert, d.h. gelbem Overlay für Gebäudeplatzierung
     * @sa #drawAt()
     */
    static const int DRAWING_FLAG_MASKED   = (1 << 0);

    /**
     * @brief zeichnet das Objekt rot eingefärbt
     * Info: Rot und abgedunkelt schließen sich gegenseitig aus.
     * @sa #drawAt()
     */
    static const int DRAWING_FLAG_RED      = (1 << 1);

    /**
     * @brief zeichnet das Objekt blinkend, d.h. entweder wird die Grafik gezeichnet oder nicht
     * @sa #drawAt()
     */
    static const int DRAWING_FLAG_BLINK    = (1 << 2);

    /**
     * @brief zeichnet das Objekt abgedunkelt
     * Info: Rot und abgedunkelt schließen sich gegenseitig aus.
     * @sa #drawAt()
     */
    static const int DRAWING_FLAG_DARKENED = (1 << 3);

public:
    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
    virtual ~IMapObjectGraphic() {}

    /**
     * @brief Liefert die Breite der Grafik in Map-Koordinaten zurück
     * @return Breite der Grafik in Map-Koordinaten
     */
    virtual unsigned char getMapWidth() const = 0;

    /**
     * @brief Liefert die Höhe der Grafik in Map-Koordinaten zurück
     * @return Breite der Höhe in Map-Koordinaten
     */
    virtual unsigned char getMapHeight() const = 0;

    /**
     * @brief Zeichnet die Grafik mit bestimmten Drawing-Flags. Quell- und Zielbereich können beliebig gewählt werden.
     * @param rectSource Quell-Rechteck der Zeichenoperation oder nullptr, wenn die komplette Grafik verwendet werden soll
     * @param rectDestination Ziel-Rechteck der Zeichenoperation oder nullptr, wenn ganzflächig gemalt werden soll
     * @param drawingFlags Bitmaske aus DRAWING_FLAG_*-Konstanten, die angeben, wie die Grafik gezeichnet werden soll
     * @param sdlTicks aktuelle Ticks aus `Context`, um den Blinke-Zustand zu kennen
     */
    virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) = 0;

};

#endif
