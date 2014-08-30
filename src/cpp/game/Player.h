#ifndef _PLAYER_H
#define _PLAYER_H

#include "SDL.h"

/**
 * Enum für die verschiedenen Spielerfarben.
 * Zugewiesene Werte sind im Format 0xAARRGGBB.
 */
enum class PlayerColor : Uint32 {
    
    RED    = 0xffff0000,
    YELLOW = 0xffffff00,
    GREEN  = 0xff00ff00,
    BLUE   = 0xff00bfff,
    
};

/**
 * @brief Klasse, die einen Spieler darstellt
 */
class Player {
    
public:
    /**
     * @brief Guthaben an Münzen
     */
    unsigned long coins;
    
private:
    /**
     * @brief Spielerfarbe
     */
    PlayerColor color;
    
    /**
     * @brief Name des Spielers
     */
    const char* name;

public:
    
    /**
     * @brief Konstruiert einen neuen Spieler
     * @param color Spielerfarbe
     * @param name Name des Spielers
     */
    Player(PlayerColor color, const char* name);
    ~Player();
    
    PlayerColor getColor() const {
        return color;
    }

    const char* getName() const {
        return name;
    }

};

#endif
