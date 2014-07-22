#include "GraphicsMgr.h"

GraphicsMgr::GraphicsMgr() {
	tiles = new Graphic*[3];
	tiles[0] = new Graphic("data/img/tiles/water.png", 1, 1);
	tiles[1] = new Graphic("data/img/tiles/grass.png", 1, 1);
	tiles[2] = new Graphic("data/img/tiles/grass2.png", 1, 1);

	objects = new Graphic*[6];
	objects[0] = new Graphic("data/img/objects/chapel.png", 2, 2);
	objects[1] = new Graphic("data/img/objects/weaponsmith.png", 2, 2);
	objects[2] = new Graphic("data/img/objects/signalfire.png", 1, 1);
	objects[3] = new Graphic("data/img/objects/herbary.png", 3, 3);
	objects[4] = new Graphic("data/img/objects/brickyard.png", 4, 2);
	objects[5] = new Graphic("data/img/objects/brickyard2.png", 2, 4);
}

GraphicsMgr::~GraphicsMgr() {
	for (int i = 0; i < 3; i++) {
		delete tiles[i];
	}
	delete[] tiles;

	for (int i = 0; i < 6; i++) {
		delete objects[i];
	}
	delete[] objects;
}
