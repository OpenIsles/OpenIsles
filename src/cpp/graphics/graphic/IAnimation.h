#ifndef _I_ANIMATION_H
#define _I_ANIMATION_H

#include "graphics/graphic/IMapObjectGraphic.h"

/**
 * @brief Interface für erweiterte Form der Map-Grafik: Die Animation enthält in einer Grafik mehrere Frames.
 * Alle Frames müssen gleich groß sein und horizontal in der Grafik aneinander gereiht sein.
 */
class IAnimation : public virtual IMapObjectGraphic {

public:
    /**
     * @brief Standard-FPS, die wir für alle Animationen benutzen sollten
     */
    static constexpr double DEFAULT_FPS = 24.0;

public:
    virtual ~IAnimation() {}


    /**
     * @brief Liefert die Breite eines Frames zurück
     * @return Breite des Frames
     */
    virtual int getWidth() const = 0;

    /**
     * @return Anzahl der Frames in der Animation
     */
    virtual unsigned int getFramesCount() const = 0;

    /**
     * @return FPS
     */
    virtual double getFps() const = 0;

    /**
     * @brief Zeichnet ein Frame skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     * @param frame Index, welches Frame gezeichnet werden soll (0 bis framesCount-1)
     */
    virtual void drawFrameScaledAt(int x, int y, double scale, unsigned int frame) = 0;

};

#endif
