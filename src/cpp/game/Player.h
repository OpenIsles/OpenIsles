#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdexcept>
#include <string>
#include "global.h"
#include "game/PlayerStatus.h"

/**
 * @page playerIndex Spieleridentifizierung
 *
 * Jeder Spieler hat eine eindeutige Farbe, die mit dem `playerIndex` gleichzusetzen ist.
 * Es werden immer für alle maximal möglichen Spieler (Player::MAX_PLAYERS) Player-Objekte angelegt.
 * Ob ein Spieler wirklich in Verwendung ist, wird durch Player::isUsed() ermittelt.
 *
 * - Im C++-Code geht der `playerIndex` von 0 bis `(Player::MAX_PLAYERS - 1)`.
 * - Im Lua-Code geht der `playerIndex` dagegen von 1 bis `(Player::MAX_PLAYERS)`, da dort Array one-based sind.
 *   Der `playerIndex` 0 steht in diesem Fall für "keinen Spieler".
 * - In der TMX-Datei werden die Spieler auch von 1 bis `(Player::MAX_PLAYERS)` durchnummiert. Diese Nummerierung
 *   hat aber nichts mit dem `playerIndex` zu tun. Der `playerIndex` wird letztlich durch die Farbe des Spieler
 *   zugeordnet.
 */

/**
 * @brief Enum für die verschiedenen Spielerfarben. Der Wert entsprechend dem (zero-based) playerIndex.
 */
enum class PlayerColor : int {
    RED = 0,
    BLUE = 1,
    YELLOW = 2,
    WHITE = 3
};

enum class PlayerType {
    NONE,    ///< dieser Spieler ist nicht im Spiel
    HUMAN,   ///< menschlicher Spieler, der die Anwendung grade bedient
    AI,      ///< KI-gesteuerter Spieler
    // TODO später hoffentlich mal NETWORK :-)
};


/**
 * @brief Klasse, die einen Spieler darstellt
 */
class Player {

public:
    /**
     * @brief maximale Spieleranzahl
     */
    static constexpr unsigned char MAX_PLAYERS = 4;

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
     * @brief Was ist das für ein Spieler
     */
    PlayerType type;
    
    /**
     * @brief Name des Spielers
     */
    std::string name;

public:
    Player() {
        type = PlayerType::NONE;
    }
    ~Player() {}

    /**
     * Setzt die Werte für einen Spieler
     *
     * @param color Spielerfarbe
     * @param type Typ des Spielers
     * @param name Name des Spielers
     * @param coins Münzguthaben
     */
    void initPlayer(PlayerColor color, const PlayerType& type, const std::string& name, long coins) {
        this->color = color;
        this->type = type;
        this->name = name;
        this->coins = coins;
    }
    
    PlayerColor getColor() const {
        return color;
    }

    /**
     * @brief Index des Spieler im Bereich 0 bis (Player::MAX_PLAYERS-1)
     */
    int getPlayerIndex() const {
        return int(color);
    }

    const std::string& getName() const {
        return name;
    }

    /**
     * @return `true`, wenn der Spieler im Spiel ist. `false`, wenn der Spieler nicht am Spiel teilnimmt.
     */
    bool isUsed() const {
        return (type != PlayerType::NONE);
    }

    /**
     * @return `true`, wenn dies ein menschlicher Spieler ist
     */
    bool isHuman() const {
        return (type == PlayerType::HUMAN);
    }

    /**
     * @return `true`, wenn dies ein KI-Spieler ist
     */
    bool isAI() const {
        return (type == PlayerType::AI);
    }
};

#endif
