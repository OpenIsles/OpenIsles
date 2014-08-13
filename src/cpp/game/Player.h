#ifndef _PLAYER_H
#define _PLAYER_H

/**
 * Enum für die verschiedenen Spielerfarben
 */
enum class PlayerColor {
    
    RED,
    YELLOW,
    GREEN,
    BLUE,
    
};

/**
 * @brief Klasse, die einen Spieler darstellt
 */
class Player {
    
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
    
    PlayerColor GetColor() const {
        return color;
    }

    const char* GetName() const {
        return name;
    }

};

#endif
