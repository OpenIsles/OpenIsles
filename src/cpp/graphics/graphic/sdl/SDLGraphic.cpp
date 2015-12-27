#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include "defines.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "utils/StringFormat.h"


SDLGraphic::SDLGraphic(IRenderer* const renderer, const char* filename) : renderer(renderer) {
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == nullptr) {
        std::fprintf(stderr, _("Could not load graphic '%s': %s\n"), filename, IMG_GetError());
        throw std::runtime_error("Could not load graphic");
    }
    this->width = surface->w;
    this->height = surface->h;
    this->surface = surface;

    createTextures();

    std::printf(_("Loaded graphic '%s': size = (%d, %d)\n"), filename, width, height);
}

SDLGraphic::SDLGraphic(
    IRenderer* const renderer, const SDLGraphic& srcGraphic, const Rect& srcRect) : renderer(renderer) {

    // Prüfen, ob das Rechteck komplett innerhalb der Grafik liegt
    if (srcRect.x + srcRect.w > srcGraphic.width ||
        srcRect.y + srcRect.h > srcGraphic.height) {

        std::fprintf(stderr, _("Illegal srcRect\n"));
        throw std::runtime_error("Illegal srcRect");
    }

    this->width = srcRect.w;
    this->height = srcRect.h;

    // Textur anlegen und Ausschnitt kopieren
    SDL_Surface* srcSurface = srcGraphic.surface;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, srcRect.w, srcRect.h,
        srcSurface->format->BitsPerPixel, srcSurface->format->Rmask,
        srcSurface->format->Gmask, srcSurface->format->Bmask, srcSurface->format->Amask);

    if (surface == nullptr) {
        std::fprintf(stderr, _("Could not create surface: %s\n"), IMG_GetError());
        throw std::runtime_error("Could not create surface");
    }

    // SDL_BlitSurface kopiert nicht 1:1, sondern verändert die Grafik an den Übergang-Rändern zur Transparenz
    // Wir kopieren deshalb den Surface-Speicher Zeile für Zeile, dass die Grafik wirklich identisch ist
    int sourceBPP = (srcSurface->format->BitsPerPixel / 8); // hier sollte immer 4 rauskommen!
    for (int dy = 0, sy = srcRect.y; dy < srcRect.h; dy++, sy++) {
        void* destPtrLine = (void*) ((unsigned char*) surface->pixels + dy * surface->pitch);
        void* srcPtrLine = (void*) ((unsigned char*) srcSurface->pixels + sy * srcSurface->pitch + srcRect.x * sourceBPP);

        memcpy(destPtrLine, srcPtrLine, (size_t) sourceBPP * srcRect.w);
    }

    this->surface = surface;

    createTextures();

    std::printf(_("Extracted graphic: srcRect = (%d, %d), %d, %d\n"), srcRect.x, srcRect.y, width, height);
}

void SDLGraphic::createTextures() {
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRealRenderer, surface);
    if (texture == nullptr) {
        std::fprintf(stderr, _("Could not create texture: %s\n"), IMG_GetError());
        throw std::runtime_error("Could not create texture");
    }
    this->texture = texture;

    textureMasked = createAlternativeTexture([&](unsigned char* pixelBuffer) {
        for (int y = 0; y < surface->h; y++) {
            uint32_t* pixelPtr = (uint32_t*) (((unsigned char*) pixelBuffer) + y * surface->pitch);

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
    });

    textureShadow = createAlternativeTexture([&](unsigned char* pixelBuffer) {
        for (int y = 0; y < surface->h; y++) {
            uint32_t* pixelPtr = (uint32_t*) (((unsigned char*) pixelBuffer) + y * surface->pitch);

            for (int x = 0; x < surface->w; x++, pixelPtr++) {
                // Halbtransparenz entfernen
                uint32_t alpha = ((*pixelPtr) & 0xff000000) >> 24;
                if (alpha >= 128) {
                    *pixelPtr = 0xff3f483f; // Pixel auf Grau setzen
                } else {
                    *pixelPtr = 0x00000000; // Pixel transparent machen
                }
            }
        }
    });
}

SDLGraphic::~SDLGraphic() {
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(textureMasked);
    SDL_DestroyTexture(textureShadow);

    width = height = -1;
    surface = nullptr;
    texture = nullptr;
    textureMasked = nullptr;
    textureShadow = nullptr;
}

SDL_Texture* SDLGraphic::createAlternativeTexture(std::function<void(unsigned char* pixelBuffer)> fillPixelsFunction) {
    // Wir können nur 32bit-Grafiken bearbeiten
    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        return nullptr;
    }

    // Pixel abkopieren und ändern
    unsigned char* pixelBuffer = new unsigned char[surface->h * surface->pitch];
    memcpy(pixelBuffer, surface->pixels, surface->h * surface->pitch);

    fillPixelsFunction(pixelBuffer);

    // Textur erstellen
    SDL_Surface* surfaceMasked = SDL_CreateRGBSurfaceFrom(
        pixelBuffer, surface->w, surface->h, surface->format->BitsPerPixel, surface->pitch,
        surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdlRealRenderer, surfaceMasked);
    SDL_FreeSurface(surfaceMasked);
    delete[] pixelBuffer;

    return sdlTexture;
}

void SDLGraphic::getPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) const {
    // Pixel finden
    SDL_PixelFormat* pixelFormat = surface->format;
    int bytesPerPixel = pixelFormat->BytesPerPixel;
    void* ptrToPixel = (void*) ((unsigned char*) surface->pixels + y * surface->pitch + x * bytesPerPixel);

    // Entsprechend Pixel-Format den Farbwert auslesen
    uint32_t pixelValue;
    uint8_t p0, p1, p2;
    switch (bytesPerPixel) {
        case 1:
            pixelValue = *(uint8_t*) ptrToPixel;
            break;
        case 2:
            pixelValue = *(uint16_t*) ptrToPixel;
            break;
        case 3:
            p0 = ((unsigned char*) ptrToPixel)[0];
            p1 = ((unsigned char*) ptrToPixel)[1];
            p2 = ((unsigned char*) ptrToPixel)[2];
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixelValue = p0 << 16 | p1 << 8 | p2;
            } else {
                pixelValue = p2 << 16 | p1 << 8 | p0;
            }
            break;
        case 4:
            pixelValue = *(uint32_t*) ptrToPixel;
            break;
        default:
            std::fprintf(stderr, _("Illegal bytesPerPixel\n"));
            throw std::runtime_error("Illegal bytesPerPixel");
    }

    // Farbwerte ermitteln
    SDL_GetRGBA(pixelValue, pixelFormat, r, g, b, a);
}

void SDLGraphic::drawAt(int x, int y) const {
    SDL_Rect rectDestination = { x, y, width, height };
    drawAt(rectDestination);
}

void SDLGraphic::drawScaledAt(int x, int y, double scale) const {
    SDL_Rect rectDestination = { x, y, (int) (getWidth() * scale), (int) (getHeight() * scale) };
    drawAt(rectDestination);
}

void SDLGraphic::drawShadowScaledAt(int x, int y, double scale) const {
    SDL_Rect rectDestination = { x, y, (int) (getWidth() * scale), (int) (getHeight() * scale) };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_SetTextureColorMod(textureShadow, 255, 255, 255);
    SDL_RenderCopy(sdlRealRenderer, textureShadow, nullptr, &rectDestination);
}

void SDLGraphic::drawShadowAt(int x, int y) const {
    drawShadowScaledAt(x, y, 1.0);
}

void SDLGraphic::drawAt(const SDL_Rect& rectDestination) const {
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_RenderCopy(sdlRealRenderer, texture, nullptr, &rectDestination);
}

void SDLGraphic::draw(Rect* rectSource, Rect* rectDestination, int drawingFlags) const {
    // normale oder maskierte Textur?
    SDL_Texture* textureToDraw;

    assert((drawingFlags & (DRAWING_FLAG_MASKED | DRAWING_FLAG_SHADOW)) != (DRAWING_FLAG_MASKED | DRAWING_FLAG_SHADOW));
    if (drawingFlags & DRAWING_FLAG_MASKED) {
        textureToDraw = textureMasked;
    } else if (drawingFlags & DRAWING_FLAG_SHADOW) {
        textureToDraw = textureShadow;
    } else {
        textureToDraw = texture;
    }

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