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
		// TODO Programm abbrechen
	}
	this->width = surface->w;
	this->height = surface->h;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		std::cerr << "Could not create texture for graphic '" << filename << "': " << SDL_GetError() << std::endl;
		// TODO Programm abbrechen
	}

	this->surface = surface;
	this->texture = texture;

	std::cout << "Loaded graphic '" << filename << "': size = (" << std::to_string(width) << ", "
			<< std::to_string(height) << ")" << std::endl;
}

Graphic::~Graphic() {
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	filename = nullptr;
	width = height = -1;
	mapWidth = mapHeight = 0;
	surface = nullptr;
	texture = nullptr;
}

SDL_Texture* Graphic::getTextureMasked() const {
    // TODO das sollte einmalig beim Anlegen der Grafik gemacht werden. Pixeloperationen sind extrem teuer!

    // Pixel abkopieren und ändern
    unsigned char* pixelsMasked = new unsigned char[surface->h * surface->pitch];
    memcpy(pixelsMasked, surface->pixels, surface->h * surface->pitch);
   
    // TODO das geht sicher besser ;-)
    for(int y = 0; y < surface->h; y++) {
        unsigned char* pixelPtr = ((unsigned char*) surface->pixels) + y * surface->pitch;
        for(int x = ((y % 2) ? 0 : 1), i = ((y % 2) ? 0 : 4); x < surface->w; x += 2, i += 8) {
            unsigned char* pixelPtrR = pixelPtr + i;
            unsigned char* pixelPtrG = pixelPtr + i + 1;
            unsigned char* pixelPtrB = pixelPtr + i + 2;
            unsigned char* pixelPtrA = pixelPtr + i + 3;
            if (*pixelPtrA > 128) {
                *pixelPtrR = 0xc8;
                *pixelPtrG = 0xaf;
                *pixelPtrB = 0x37;
                *pixelPtrA = 255;
            }
        }
    }
   
    // Textur erstellen
    SDL_Surface* surfaceMasked = SDL_CreateRGBSurfaceFrom(
            pixelsMasked, surface->w, surface->h, surface->format->BitsPerPixel, surface->pitch,
            surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    SDL_Texture* textureMasked = SDL_CreateTextureFromSurface(renderer, surfaceMasked);
    SDL_FreeSurface(surfaceMasked);
    delete[] pixelsMasked;

    return textureMasked;
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

// TODO sollte später wieder wegfliegen oder zumindest nicht mehr public sein
void Graphic::setPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    // aus den Farbwerte zu setzenden Pixelwert ermitteln
    SDL_PixelFormat* pixelFormat = surface->format;
    Uint32 pixelValue = SDL_MapRGBA(pixelFormat, r, g, b, a);
    
    // Pixel finden
	int bytesPerPixel = pixelFormat->BytesPerPixel;
	void* ptrToPixel = (void*) ((Uint8*) surface->pixels + y * surface->pitch + x * bytesPerPixel);
    
	// Entsprechend Pixel-Format den Pixel setzen
	switch (bytesPerPixel) {
		case 1:
			*(Uint8*) ptrToPixel = (Uint8) pixelValue;
			break;
		case 2:
			*(Uint16*) ptrToPixel = (Uint16) pixelValue;
			break;
		case 3: // fall through
		case 4:
            *(Uint32*) ptrToPixel = pixelValue;
			break;
	}
}