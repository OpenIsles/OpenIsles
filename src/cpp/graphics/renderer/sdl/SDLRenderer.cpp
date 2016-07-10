#include <cstdlib>
#include "global.h"
#include "graphics/renderer/sdl/SDLRenderer.h"

// Fenster-Größe
static const int WINDOW_WIDTH = 1024;
static const int WINDOW_HEIGHT = 768;


SDLRenderer::SDLRenderer() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        Log::error(_("SDL could not be initialized: %s"), SDL_GetError());
        return;
    }

    atexit(SDL_Quit);
    Log::info(_("SDL initialized."));

    SDL_Surface* surfaceAppIcon = IMG_Load("data/img/appicon.png");
    if (surfaceAppIcon == nullptr) {
        Log::error(_("Could not load graphic 'data/img/appicon.png': %s"), SDL_GetError());
        return;
    }

    window = SDL_CreateWindow(
         "OpenIsles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_HIDDEN);
    if (window == nullptr) {
        Log::error(_("SDL could not create window: %s"), SDL_GetError());
        return;
    }

    SDL_SetWindowIcon(window, surfaceAppIcon);
    SDL_FreeSurface(surfaceAppIcon);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr) {
        Log::error(_("SDL could not create renderer: %s"), SDL_GetError());
        SDL_DestroyWindow(window);
        return;
    }

    offscreenTexture = SDL_CreateTexture(
         renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        Log::error(_("Could not init SDL-image: %s"), IMG_GetError());
        return;
    }
    atexit(IMG_Quit);

    if (TTF_Init() != 0) {
        Log::error(_("Could not init SDL-TTF: %s"), TTF_GetError());
        return;
    }
    atexit(TTF_Quit);
}

SDLRenderer::~SDLRenderer() {
    SDL_DestroyTexture(offscreenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLRenderer::showWindow() {
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
}

const int SDLRenderer::getWindowWidth() const {
    return WINDOW_WIDTH;
}

const int SDLRenderer::getWindowHeight() const {
    return WINDOW_HEIGHT;
}

void SDLRenderer::setClipRect(const Rect* const rect) {
    if (rect != nullptr) {
#if SDL_VERSION_ATLEAST(2, 0, 4) || defined(WINDOWS)
        SDL_Rect sdlRect = { rect->x, rect->y, rect->w, rect->h };
#else
        /* Workaround für Bug in SDL (bis v2.0.3)
         * @see https://bugzilla.libsdl.org/show_bug.cgi?id=2700
         *
         * SDL misst das Rechteck unter Linux von UNTEN. Unter Windows verhält sich SDL_RenderSetClipRect() wie erwartet.
         * Problem ist, dass SDL nicht korrekt vom OpenGL-Koordinatensystem abstrahiert und das hat den Nullpunkt
         * unten-links.
         */
        int fixedY = getWindowHeight() - (rect->y + rect->h);
        SDL_Rect sdlRect = { rect->x, fixedY, rect->w, rect->h };
#endif

        SDL_RenderSetClipRect(renderer, &sdlRect);
    } else {
        SDL_RenderSetClipRect(renderer, nullptr);
    }
}

void SDLRenderer::setDrawColor(const Color& color) {
    if (color.a < 255) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    } else {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void SDLRenderer::setDrawBlendMode(int blendMode) {
    SDL_BlendMode sdlBlendMode = SDL_BLENDMODE_NONE;

    if (blendMode == BLENDMODE_NONE) {
        sdlBlendMode = SDL_BLENDMODE_NONE;
    } else if (blendMode == BLENDMODE_BLEND) {
        sdlBlendMode = SDL_BLENDMODE_BLEND;
    } else if (blendMode == BLENDMODE_ADD) {
        sdlBlendMode = SDL_BLENDMODE_ADD;
    } else if (blendMode == BLENDMODE_MOD) {
        sdlBlendMode = SDL_BLENDMODE_MOD;
    }

    SDL_SetRenderDrawBlendMode(renderer, sdlBlendMode);
}

void SDLRenderer::drawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void SDLRenderer::drawRect(const Rect& rect) {
    SDL_Rect sdlRect = { rect.x, rect.y, rect.w, rect.h };
    SDL_RenderDrawRect(renderer, &sdlRect);
}

void SDLRenderer::fillRect(const Rect& rect) {
    SDL_Rect sdlRect = { rect.x, rect.y, rect.w, rect.h };
    SDL_RenderFillRect(renderer, &sdlRect);
}

void SDLRenderer::setHintRenderScaleQuality(const char* scaleQuality) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQuality);
}

void SDLRenderer::startFrame() {
    // Frame auf Offscreen-Texture zeichnen
    SDL_SetRenderTarget(renderer, offscreenTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255);
    SDL_RenderClear(renderer);
}

void SDLRenderer::endFrame() {
    // Frame fertig. Erst jetzt komplett in Fenster übertragen, um Flackern zu unterbinden
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderCopy(renderer, offscreenTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void* SDLRenderer::takeScreenshot() {
    int width, height;
    SDL_QueryTexture(offscreenTexture, nullptr, nullptr, &width, &height);

    char* pixels = new char[3 * width * height];

    if (SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGB24, pixels, 3 * width) != 0) {
        delete[] pixels;
        return nullptr;
    }

    SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, 24, 3 * width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000);
    return sdlSurface;
}

SDL_Renderer* SDLRenderer::getRealRenderer() {
    return renderer;
}
