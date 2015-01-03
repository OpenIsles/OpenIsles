#include <cstring>
#include <iostream>
#include <stdexcept>
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "utils/StringFormat.h"


SDLGraphic::SDLGraphic(IRenderer* const renderer, const char* filename) : SDLGraphic(renderer, filename, 0, 0) {}

SDLGraphic::SDLGraphic(IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight) :
	renderer(renderer) {

	this->mapWidth = mapWidth;
	this->mapHeight = mapHeight;

	SDL_Surface* surface = IMG_Load(filename);
	if (surface == nullptr) {
		std::cerr << "Could not load graphic '" << filename << "': " << IMG_GetError() << std::endl;
		throw new std::runtime_error("Could not load graphic");
	}
	this->width = surface->w;
	this->height = surface->h;
	this->surface = surface;

	createTextures();

	std::cout << "Loaded graphic '" << filename << "': size = (" << toString(width) << ", "
			<< toString(height) << ")" << std::endl;
}

SDLGraphic::SDLGraphic(IRenderer* const renderer, const SDLGraphic& srcGraphic, const Rect& srcRect) :
	SDLGraphic(renderer, srcGraphic, srcRect, 0, 0) {}


SDLGraphic::SDLGraphic(IRenderer* const renderer, const SDLGraphic& srcGraphic, const Rect& srcRect,
	                   unsigned char mapWidth, unsigned char mapHeight) :
	renderer(renderer) {

	this->mapWidth = mapWidth;
	this->mapHeight = mapHeight;

	// Prüfen, ob das Rechteck komplett innerhalb der Grafik liegt
	if (srcRect.x + srcRect.w > srcGraphic.width ||
		srcRect.y + srcRect.h > srcGraphic.height) {

		std::cerr << "Illegal srcRect" << std::endl;
		throw new std::runtime_error("Illegal srcRect");
	}

	this->width = srcRect.w;
	this->height = srcRect.h;

	// Textur anlegen und Ausschnitt kopieren
	SDL_Surface* srcSurface = srcGraphic.surface;
	SDL_Surface* surface = SDL_CreateRGBSurface(srcSurface->flags, srcRect.w, srcRect.h,
		srcSurface->format->BitsPerPixel, srcSurface->format->Rmask,
		srcSurface->format->Gmask, srcSurface->format->Bmask, srcSurface->format->Amask);

	if (surface == nullptr) {
		std::cerr << "Could not create surface: " << IMG_GetError() << std::endl;
		throw new std::runtime_error("Could not create surface");
	}

	SDL_Rect sdlSrcRect = { srcRect.x, srcRect.y, srcRect.w, srcRect.h };
	if (SDL_BlitSurface(srcSurface, &sdlSrcRect, surface, nullptr) != 0) {
		std::cerr << "Could not copy surface data: " << IMG_GetError() << std::endl;
		throw new std::runtime_error("Could not copy surface data");
	}

	this->surface = surface;

	createTextures();

	std::cout << "Extracted graphic: srcRect = (" << toString(srcRect.x) << ", " << toString(srcRect.y) << "), " <<
		toString(width) << ", " << toString(height) << ")" << std::endl;
}

void SDLGraphic::createTextures() {
	SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRealRenderer, surface);
	if (texture == nullptr) {
		std::cerr << "Could not create texture" << SDL_GetError() << std::endl;
		throw new std::runtime_error("Could not create texture");
	}

	this->texture = texture;

	createMaskedTexture();
}

SDLGraphic::~SDLGraphic() {
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(textureMasked);

	width = height = -1;
	surface = nullptr;
	texture = nullptr;
	textureMasked = nullptr;
}

void SDLGraphic::createMaskedTexture() {
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
            throw new std::runtime_error("Illegal bytesPerPixel");
	}

	// Farbwerte ermitteln
	SDL_GetRGBA(pixelValue, pixelFormat, r, g, b, a);
}

void SDLGraphic::drawAt(int x, int y) const {
	SDL_Rect rectDestination = { x, y, width, height };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, texture, nullptr, &rectDestination);
}

void SDLGraphic::drawScaledAt(int x, int y, double scale) const {
	SDL_Rect rectDestination = { x, y, (int) (getWidth() * scale), (int) (getHeight() * scale) };
    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    SDL_RenderCopy(sdlRealRenderer, texture, nullptr, &rectDestination);
}

void SDLGraphic::draw(Rect* rectSource, Rect* rectDestination, int drawingFlags, uint32_t sdlTicks) const {
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