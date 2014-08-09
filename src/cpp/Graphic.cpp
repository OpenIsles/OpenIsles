#include <stdexcept>
#include <iostream>
#include "Graphic.h"

extern SDL_Renderer* renderer;

Graphic::Graphic(const char* filename) : Graphic(filename, 0, 0) {
}

Graphic::Graphic(const char* filename, unsigned char mapWidth, unsigned char mapHeight) {
	this->filename = filename;
	this->mapWidth = mapWidth;
	this->mapHeight = mapHeight;

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
    
    createMaskedTexture();

	std::cout << "Loaded graphic '" << filename << "': size = (" << std::to_string(width) << ", "
			<< std::to_string(height) << ")" << std::endl;
}

Graphic::~Graphic() {
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
    if (textureMasked != nullptr) {
        SDL_DestroyTexture(textureMasked);
    }

	filename = nullptr;
	width = height = -1;
	mapWidth = mapHeight = 0;
	surface = nullptr;
	texture = nullptr;
    textureMasked = nullptr;
}

void Graphic::createMaskedTexture() {
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
	}

	// Farbwerte ermitteln
	SDL_GetRGBA(pixelValue, pixelFormat, r, g, b, a);
}