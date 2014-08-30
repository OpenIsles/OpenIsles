#include <string.h>
#include "game/Player.h"


Player::Player(PlayerColor color, const char* name) : coins(10000), color(color), name(name) {  
}

Player::~Player() {
}