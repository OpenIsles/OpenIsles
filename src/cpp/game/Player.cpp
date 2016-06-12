#include "game/Player.h"


Player::Player(PlayerColor color, const PlayerType& type, const std::string& name) :
    coins(10000), color(color), type(type), name(name) {

}

Player::~Player() {
}