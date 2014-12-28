#ifdef WINDOWS
#include <cstring>
#endif

#include "graphics/graphic/sdl/SDLMapObjectGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"


SDLMapObjectGraphic::SDLMapObjectGraphic(
    IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight) :
    SDLGraphic(renderer, filename),
    SDLPlainGraphic(renderer, filename),
    AbstractMapObjectGraphic(mapWidth, mapHeight)
{
    createMaskedTexture();
}

SDLMapObjectGraphic::~SDLMapObjectGraphic() {
    if (textureMasked != nullptr) {
        SDL_DestroyTexture(textureMasked);
    }

    mapWidth = mapHeight = 0;
    textureMasked = nullptr;
}

void SDLMapObjectGraphic::createMaskedTexture() {
    // Wir können nur 32bit-Grafiken bearbeiten
    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        textureMasked = nullptr;
        return;
    }

    // Pixel abkopieren und ändern
    unsigned char* pixelsMasked = new unsigned char[surface->h * surface->pitch];
    memcpy(pixelsMasked, surface->pixels, surface->h * surface->pitch);

    uint32_t* pixelPtr = (uint32_t*) pixelsMasked;
    for (int y = 0; y < surface->h; y++) {
        pixelPtr = (uint32_t*) (((unsigned char*) pixelsMasked) + y * surface->pitch);

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

    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    textureMasked = SDL_CreateTextureFromSurface(sdlRealRenderer, surfaceMasked);
    SDL_FreeSurface(surfaceMasked);
    delete[] pixelsMasked;
}

void SDLMapObjectGraphic::draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) {
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

    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();

    SDL_Rect* sdlRectSource;
    if (rectSource != nullptr) {
        sdlRectSource = new SDL_Rect{ rectSource->x, rectSource->y, rectSource->w, rectSource->h };
    } else {
        sdlRectSource = nullptr;
    }

    SDL_Rect* sdlRectDestination;
    if (rectDestination != nullptr) {
        sdlRectDestination = new SDL_Rect
            { rectDestination->x, rectDestination->y, rectDestination->w, rectDestination->h };
    } else {
        sdlRectDestination = nullptr;
    }

    SDL_RenderCopy(sdlRealRenderer, textureToDraw, sdlRectSource, sdlRectDestination);

    if (sdlRectSource != nullptr) {
        delete sdlRectSource;
    }
    if (sdlRectDestination != nullptr) {
        delete sdlRectDestination;
    }
}
