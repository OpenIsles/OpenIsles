#include <string.h>
#include "game/Player.h"


Player::Player(PlayerColor color, const char* name) : color(color), name(name), coins(10000) {  
}

Player::~Player() {
}