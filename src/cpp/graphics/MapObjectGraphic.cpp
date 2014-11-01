#ifdef WINDOWS
#include <cstring>
#endif
#include "graphics/MapObjectGraphic.h"

// Aus main.cpp importiert
extern SDL_Renderer* renderer;
extern Uint32 sdlTicks;


MapObjectGraphic::MapObjectGraphic(const char* filename, unsigned char mapWidth, unsigned char mapHeight) :
    PlainGraphic(filename), mapWidth(mapWidth), mapHeight(mapHeight) {

    createMaskedTexture();
}

MapObjectGraphic::~MapObjectGraphic() {
    if (textureMasked != nullptr) {
        SDL_DestroyTexture(textureMasked);
    }

    mapWidth = mapHeight = 0;
    textureMasked = nullptr;
}

void MapObjectGraphic::createMaskedTexture() {
    // Wir können nur 32bit-Grafiken bearbeiten
    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        textureMasked = nullptr;
        return;
    }

    // Pixel abkopieren und ändern
    unsigned char* pixelsMasked = new unsigned char[surface->h * surface->pitch];
    memcpy(pixelsMasked, surface->pixels, surface->h * surface->pitch);

    Uint32* pixelPtr = (Uint32*) pixelsMasked;
    for (int y = 0; y < surface->h; y++) {
        pixelPtr = (Uint32*) (((unsigned char*) pixelsMasked) + y * surface->pitch);

        int x = 0;
        if (y % 2) {
            x = 1;
            pixelPtr++;
        }

        for (; x < surface->w; x += 2, pixelPtr += 2) {
            *pixelPtr &= 0xff000000; // Alpha-Kanel unverändert lassen
            *pixelPtr |= 0x0037afc8; // Pixelfarbe auf Orange setzen
        }
    }

    // Textur erstellen
    SDL_Surface* surfaceMasked = SDL_CreateRGBSurfaceFrom(
        pixelsMasked, surface->w, surface->h, surface->format->BitsPerPixel, surface->pitch,
        surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    textureMasked = SDL_CreateTextureFromSurface(renderer, surfaceMasked);
    SDL_FreeSurface(surfaceMasked);
    delete[] pixelsMasked;
}

void MapObjectGraphic::draw(SDL_Rect* rectSource, SDL_Rect* rectDestination, int drawingFlags) {
    // Blinkmodus? Grafik nur in der ersten Hälfte eines Intervalls zeichnen
    if ((drawingFlags & DRAWING_FLAG_BLINK) && (sdlTicks % 800 < 400)) {
        return;
    }

    // normale oder maskierte Textur?
    SDL_Texture* textureToDraw = (drawingFlags & DRAWING_FLAG_MASKED) ? textureMasked : texture;

    // Abgedunkelt oder rot zeichnen?
    if (drawingFlags & DRAWING_FLAG_DARKENED) {
        SDL_SetTextureColorMod(textureToDraw, 160, 160, 160);
    } else if (drawingFlags & DRAWING_FLAG_RED) {
        SDL_SetTextureColorMod(textureToDraw, 255, 0, 0);
    } else {
        SDL_SetTextureColorMod(textureToDraw, 255, 255, 255);
    }

    SDL_RenderCopy(renderer, textureToDraw, rectSource, rectDestination);
}
