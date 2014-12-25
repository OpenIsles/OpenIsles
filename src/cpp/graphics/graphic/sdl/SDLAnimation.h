#ifndef _SDL_ANIMATION_H
#define _SDL_ANIMATION_H

#include "graphics/graphic/IAnimation.h"
#include "graphics/graphic/sdl/SDLMapObjectGraphic.h"

/**
 * @brief Erweiterte Form der Map-Grafik: Die Animation enthält in einer Grafik mehrere Frames.
 * Alle Frames müssen gleich groß sein und horizontal in der Grafik aneinander gereiht sein.
 */
class SDLAnimation : public SDLMapObjectGraphic, public virtual IAnimation {

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
     * @param renderer (Dependency)
     * @param filename Dateiname der zu ladenden Animationsgrafik
     * @param mapWidth Breite in mapCoords
     * @param mapHeight Höhe in mapCoords
     * @param framesCount Anzahl der Frames, die sich in dieser Datei befinden
     * @param fps FPS = Geschwindigkeit, wie viele Frames pro Sekunde abgespielt werden sollen
     */
    SDLAnimation(IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight,
              unsigned int framesCount, double fps);

    virtual ~SDLAnimation() {}


    /**
     * @brief Liefert die Breite eines Frames zurück
     * @return Breite des Frames
     */
    virtual int getWidth() const override {
        return frameWidth;
    }

    /**
     * @return Anzahl der Frames in der Animation
     */
    virtual unsigned int getFramesCount() const {
        return framesCount;
    }

    /**
     * @return FPS
     */
    virtual double getFps() const {
        return fps;
    }

    /**
     * @brief Zeichnet ein Frame skaliert
     * @param x x-Koordinate, wo hingezeichnet werden soll
     * @param y y-Koordinate, wo hingezeichnet werden soll
     * @param scale Skalierungsfaktor
     * @param frame Index, welches Frame gezeichnet werden soll (0 bis framesCount-1)
     */
    virtual void drawFrameScaledAt(int x, int y, double scale, unsigned int frame);

};

#endif
