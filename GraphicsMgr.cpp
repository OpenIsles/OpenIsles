#include "GraphicsMgr.h"

GraphicsMgr::GraphicsMgr(SDL_Renderer* renderer) {
	this->renderer = renderer;

	tileTextures = new SDL_Texture*[3];
	initTileTexture(0, "water.png");
	initTileTexture(1, "grass.png");
	initTileTexture(2, "grass2.png");

	objectTextures = new SDL_Texture*[2];
	initObjectTexture(0, "chapel.png");
	initObjectTexture(1, "weaponsmith.png");
}

GraphicsMgr::~GraphicsMgr() {
	for (int i = 0; i < 3; i++) {
		SDL_DestroyTexture(tileTextures[i]);
	}
	delete[] tileTextures;

	for (int i = 0; i < 2; i++) {
		SDL_DestroyTexture(objectTextures[i]);
	}
	delete[] objectTextures;
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

void GraphicsMgr::initObjectTexture(int index, const char* filename) {
	std::string filePath = "data/img/objects/";
	filePath += filename;

	SDL_Surface* surfaceObject = IMG_Load(filePath.data());
	if (surfaceObject == nullptr) {
		std::cerr << "Could not load object '" << filename << "': " << IMG_GetError() << std::endl;
		// TODO Programm abbrechen
	}

	SDL_Texture* textureObject = SDL_CreateTextureFromSurface(renderer, surfaceObject);
	if (textureObject == nullptr) {
		std::cerr << "Could not create texture for object '" << filename << "': " << SDL_GetError() << std::endl;
		// TODO Programm abbrechen
	}
	SDL_FreeSurface(surfaceObject);

	objectTextures[index] = textureObject;
}
