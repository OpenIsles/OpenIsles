#include <stdexcept>
#include <iostream>
#include "graphics/Graphic.h"
#include "utils/StringFormat.h"

extern SDL_Renderer* renderer;

Graphic::Graphic(const char* filename) {
	this->filename = filename;

	SDL_Surface* surface = IMG_Load(filename);
	if (surface == nullptr) {
		std::cerr << "Could not load graphic '" << filename << "': " << IMG_GetError() << std::endl;
		throw new std::runtime_error("Could not load graphic");
	}
	this->width = surface->w;
	this->height = surface->h;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		std::cerr << "Could not create texture for graphic '" << filename << "': " << SDL_GetError() << std::endl;
		throw new std::runtime_error("Could not create texture");
	}

	this->surface = surface;
	this->texture = texture;
    
	std::cout << "Loaded graphic '" << filename << "': size = (" << toString(width) << ", "
			<< toString(height) << ")" << std::endl;
}

Graphic::~Graphic() {
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	filename = nullptr;
	width = height = -1;
	surface = nullptr;
	texture = nullptr;
}

void Graphic::getPixel(int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a) {
	// Pixel finden
	SDL_PixelFormat* pixelFormat = surface->format;
	int bytesPerPixel = pixelFormat->BytesPerPixel;
	void* ptrToPixel = (void*) ((Uint8*) surface->pixels + y * surface->pitch + x * bytesPerPixel);

	// Entsprechend Pixel-Format den Farbwert auslesen
	Uint32 pixelValue;
	Uint8 p0, p1, p2;
	switch (bytesPerPixel) {
		case 1:
			pixelValue = *(Uint8*) ptrToPixel;
			break;
		case 2:
			pixelValue = *(Uint16*) ptrToPixel;
			break;
		case 3:
			p0 = ((Uint8*) ptrToPixel)[0];
			p1 = ((Uint8*) ptrToPixel)[1];
			p2 = ((Uint8*) ptrToPixel)[2];
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixelValue = p0 << 16 | p1 << 8 | p2;
			} else {
				pixelValue = p2 << 16 | p1 << 8 | p0;
			}
			break;
		case 4:
			pixelValue = *(Uint32*) ptrToPixel;
			break;
        default:
            throw new std::runtime_error("Illegal bytesPerPixel");
	}

	// Farbwerte ermitteln
	SDL_GetRGBA(pixelValue, pixelFormat, r, g, b, a);
}