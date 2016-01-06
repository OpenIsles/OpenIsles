#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdexcept>
#include <string>
#include "global.h"
#include "game/PlayerStatus.h"

/**
 * @brief Enum für die verschiedenen Spielerfarben.
 * Zugewiesene Werte sind im Format 0xAARRGGBB.
 */
enum class PlayerColor : unsigned int {
    
    RED    = 0xffaf2707,
    BLUE   = 0xff1088df,
    YELLOW = 0xfff8c030,
    WHITE  = 0xffc0c0c0,
    
};

// Defines, um Offsets für die Spielerfarben auf Bezeichner anwenden zu können, siehe Player::getColorIndex().
#define PLAYER_RED    0
#define PLAYER_BLUE   1
#define PLAYER_YELLOW 2
#define PLAYER_WHITE  3

/**
 * @brief Klasse, die einen Spieler darstellt
 */
class Player {

    friend class Game; // Beim Hinzufügen eines Spielers wird der index gesetzt
    
public:
    /**
     * @brief Guthaben an Münzen
     */
    long coins;

    /**
     * @brief "Spielerstatus"-Informationen
     */
    PlayerStatus playerStatus;
    
private:
    /**
     * @brief Spielerfarbe
     */
    PlayerColor color;
    
    /**
     * @brief Name des Spielers
     */
    const std::string name;

public:
    
    /**
     * @brief Konstruiert einen neuen Spieler
     * @param color Spielerfarbe
     * @param name Name des Spielers
     */
    Player(PlayerColor color, const std::string& name);
    ~Player();
    
    PlayerColor getColor() const {
        return color;
    }

    const std::string& getName() const {
        return name;
    }

    /**
     * @brief Liefert des Farbindex des Spielers zurück. Das entspricht dem Wert einer der Defines `PLAYER_*`.
     * @return Farbindex des Spielers im Bereich von 0 bis 3.
     */
    int getColorIndex() const {
        if (color == PlayerColor::RED) {
            return PLAYER_RED;
        } else if (color == PlayerColor::BLUE) {
            return PLAYER_BLUE;
        } else if (color == PlayerColor::YELLOW) {
            return PLAYER_YELLOW;
        } else if (color == PlayerColor::WHITE) {
            return PLAYER_WHITE;
        }

        Log::error(_("Illegal player color: %u"), (unsigned int) color);
        throw std::runtime_error("Illegal player color");
    }
};

#endif
