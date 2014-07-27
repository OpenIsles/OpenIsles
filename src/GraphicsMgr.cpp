#include <fstream>
#include <sstream>
#include <stdexcept>
#include "GraphicsMgr.h"

GraphicsMgr::GraphicsMgr() {
	loadTiles();

	objects = new Graphic*[6];
	objects[0] = new Graphic("data/img/objects/chapel.png", 2, 2);
	objects[1] = new Graphic("data/img/objects/weaponsmith.png", 2, 2);
	objects[2] = new Graphic("data/img/objects/signalfire.png", 1, 1);
	objects[3] = new Graphic("data/img/objects/herbary.png", 3, 3);
	objects[4] = new Graphic("data/img/objects/brickyard.png", 4, 2);
	objects[5] = new Graphic("data/img/objects/brickyard2.png", 2, 4);
}

GraphicsMgr::~GraphicsMgr() {
	for (int i = 0; i < 128; i++) {
		if (tiles[i] != nullptr) {
			delete tiles[i];
		}
	}
	delete[] tiles;

	for (int i = 0; i < 6; i++) {
		delete objects[i];
	}
	delete[] objects;
}

void GraphicsMgr::loadTiles() {
	tiles = new Graphic*[128];

	std::ifstream inputFileStreamTilesTxt("data/img/tiles/tiles.txt");
	std::string line;
	int lineNr = 0;
	while (std::getline(inputFileStreamTilesTxt, line)) {
		lineNr++;

		// Leerzeilen und Kommentarzeilen überspringen
		if (line.empty() || line.data()[0] == '#')
			continue;

		std::istringstream inputStringStream(line);
		int tileIndex, xOffset, yOffset;
		std::string tileFilename;
		if (!(inputStringStream >> tileIndex >> xOffset >> yOffset >> tileFilename)) {
			throw new std::runtime_error("Error in tiles.txt line " + lineNr);
		}

		std::string tileFilepath("data/img/tiles/" + tileFilename);
		tiles[tileIndex] = new Graphic(tileFilepath.data(), 1, 1);
	}
}
