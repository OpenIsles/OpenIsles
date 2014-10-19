#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "graphics/PlainGraphic.h"

/**
 * @brief Erweiterte Form der Grafik: Die Animation enthält in einer Grafik mehrere Frames.
 * Alle Frames müssen gleich groß sein und horizontal in der Grafik aneinander gereiht sein.
 */
class Animation : public PlainGraphic {

public:
    /**
     * @brief Standard-FPS, die wir für alle Animationen benutzen sollten
     */
    static constexpr double DEFAULT_FPS = 24.0;

private:
    /**
     * @brief Gesamtzahl der Frames
     */
    unsigned int framesCount;

    /**
     * @brief Breite eines Frames.
     * Es gilt frameWidth * framesCount == width
     */
    int frameWidth;

    /**
     * @brief FPS = Geschwindigkeit, wie viele Frames pro Sekunde abgespielt werden sollen
     */
    double fps;

public:
    /**
     * @brief Konstruktor für eine Animation
     * @param filename Dateiname der zu ladenden Animationsgrafik
     * @param framesCount Anzahl der Frames, die sich in dieser Datei befinden
     * @param fps FPS = Geschwindigkeit, wie viele Frames pro Sekunde abgespielt werden sollen
     */
    Animation(const char* filename, unsigned int framesCount, double fps);

    ~Animation() {}

    /**
     * @brief Zeichnet ein Frame
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param frame Index, welches Frame gezeichnet werden soll (0 bis framesCount-1)
     */
    void drawFrameAt(int x, int y, unsigned int frame);

};

#endif
