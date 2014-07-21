#include "GraphicsMgr.h"

GraphicsMgr::GraphicsMgr() {
	tiles = new Graphic*[3];
	tiles[0] = new Graphic("data/img/tiles/water.png");
	tiles[1] = new Graphic("data/img/tiles/grass.png");
	tiles[2] = new Graphic("data/img/tiles/grass2.png");

	objects = new Graphic*[2];
	objects[0] = new Graphic("data/img/objects/chapel.png");
	objects[1] = new Graphic("data/img/objects/weaponsmith.png");
}

GraphicsMgr::~GraphicsMgr() {
	for (int i = 0; i < 3; i++) {
		delete tiles[i];
	}
	delete[] tiles;

	for (int i = 0; i < 2; i++) {
		delete objects[i];
	}
	delete[] objects;
}
