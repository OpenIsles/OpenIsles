#ifndef _SDL_MAP_OBJECT_GRAPHIC_H
#define _SDL_MAP_OBJECT_GRAPHIC_H

#include <SDL.h>
#include "graphics/graphic/sdl/SDLPlainGraphic.h"
#include "graphics/graphic/AbstractMapObjectGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "utils/Rect.h"

/**
 * @brief Grafik eines MapObjects. Die Grafik hat eine fixe Größe in Map-Koordinaten zugeordnet, sodass sie in das
 * Kachelsystem der Map gezeichnet werden kann
 */
class SDLMapObjectGraphic : public virtual SDLPlainGraphic, public virtual AbstractMapObjectGraphic {

private:
    /**
     * @brief SDL-Texture der maskierten Grafik zurück. Diese wird verwendet, wenn ein neues Gebäude
     * positioniert wird.
     */
    SDL_Texture* textureMasked;

public:
    /**
     * @brief Konstruktor. Lädt die Grafik.
     * @param renderer (Dependency)
     * @param filename Dateiname der zu ladenden Grafik
     * @param mapWidth Breite in mapCoords
     * @param mapHeight Höhe in mapCoords
     */
    SDLMapObjectGraphic(
        IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight);

    /**
     * @brief Destruktor. Entlädt die Grafik und gibt Speicher frei.
     */
    virtual ~SDLMapObjectGraphic();

    /**
     * @brief Zeichnet die Grafik mit bestimmten Drawing-Flags. Quell- und Zielbereich können beliebig gewählt werden.
     * @param rectSource Quell-Rechteck der Zeichenoperation oder nullptr, wenn die komplette Grafik verwendet werden soll
     * @param rectDestination Ziel-Rechteck der Zeichenoperation oder nullptr, wenn ganzflächig gemalt werden soll
     * @param drawingFlags Bitmaske aus DRAWING_FLAG_*-Konstanten, die angeben, wie die Grafik gezeichnet werden soll
     * @param sdlTicks aktuelle Ticks aus `Context`, um den Blinke-Zustand zu kennen
     */
    virtual void draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks);

private:
    /**
     * @brief Erzeugt @ref textureMasked. Erwartet, dass @ref surface bereits mit der Grafik befüllt ist.
     */
    void createMaskedTexture();

};

#endif
