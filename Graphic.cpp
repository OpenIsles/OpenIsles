#include <iostream>
#include "Graphic.h"

extern SDL_Renderer* renderer;

Graphic::Graphic(const char* filename) {
	this->filename = filename;

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
	SDL_FreeSurface(surface);

	// TODO Surface dazu benutzen, eine Alphamasking anzufertigen, damit wir später gucken können, ob eine Grafik
	// angeklickt wurde oder ins Transparente geklickt wurde

	this->texture = texture;

	std::cout << "Loaded graphic '" << filename << "': size = (" << std::to_string(width) << ", "
			<< std::to_string(height) << ")" << std::endl;
}

Graphic::~Graphic() {
	SDL_DestroyTexture(texture);

	filename = nullptr;
	width = height = 0;
	texture = nullptr;
}

