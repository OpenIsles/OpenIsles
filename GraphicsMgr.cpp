#include "GraphicsMgr.h"


GraphicsMgr::GraphicsMgr(SDL_Renderer* renderer) {
	this->renderer = renderer;

	tileTextures = new SDL_Texture*[3];

	initTileTexture(0, "water.png");
	initTileTexture(1, "grass.png");
	initTileTexture(2, "grass2.png");
}


GraphicsMgr::~GraphicsMgr() {
	for (int i = 0; i < 3; i++) {
		SDL_DestroyTexture(tileTextures[i]);
	}
	delete[] tileTextures;
}


void GraphicsMgr::initTileTexture(int index, const char* filename) {
	std::string filePath = "data/img/tiles/";
	filePath += filename;

	SDL_Surface* surfaceTile = IMG_Load(filePath.data());
	if (surfaceTile == nullptr) {
		std::cerr << "Could not load tile '" << filename << "': " << IMG_GetError() << std::endl;
		// TODO Programm abbrechen
	}

	SDL_Texture* textureTile = SDL_CreateTextureFromSurface(renderer, surfaceTile);
	if (textureTile == nullptr) {
		std::cerr << "Could not create texture for tile '" << filename << "': " << SDL_GetError() << std::endl;
		// TODO Programm abbrechen
	}
	SDL_FreeSurface(surfaceTile);

	tileTextures[index] = textureTile;
}
