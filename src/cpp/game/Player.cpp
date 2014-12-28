#include "game/Player.h"


Player::Player(PlayerColor color, const std::string& name) : coins(10000), color(color), name(name) {
}

Player::~Player() {
}