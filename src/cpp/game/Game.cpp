#include "map/Building.h"
#include "game/Game.h"

Game::Game() {
}

Game::~Game() {
    for (auto iter = players.cbegin(); iter != players.cend(); iter++) {
		Player* player = *iter;
		delete player;
	}
	players.clear();
}