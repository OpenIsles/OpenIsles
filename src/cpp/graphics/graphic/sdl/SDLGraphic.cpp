#include <stdexcept>
#include <iostream>
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "utils/StringFormat.h"


SDLGraphic::SDLGraphic(IRenderer* const renderer, const char* filename) : renderer(renderer), filename(filename) {
	SDL_Surface* surface = IMG_Load(filename);
	if (surface == nullptr) {
		std::cerr << "Could not load graphic '" << filename << "': " << IMG_GetError() << std::endl;
		throw new std::runtime_error("Could not load graphic");
	}
	this->width = surface->w;
	this->height = surface->h;

	SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRealRenderer, surface);
	if (texture == nullptr) {
		std::cerr << "Could not create texture for graphic '" << filename << "': " << SDL_GetError() << std::endl;
		throw new std::runtime_error("Could not create texture");
	}

	this->surface = surface;
	this->texture = texture;
    
	std::cout << "Loaded graphic '" << filename << "': size = (" << toString(width) << ", "
			<< toString(height) << ")" << std::endl;
}

SDLGraphic::~SDLGraphic() {
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	filename = nullptr;
	width = height = -1;
	surface = nullptr;
	texture = nullptr;
}

void SDLGraphic::getPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
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