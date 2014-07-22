#include <iostream>
#include "Graphic.h"

extern SDL_Renderer* renderer;

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

