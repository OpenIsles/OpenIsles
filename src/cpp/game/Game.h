#ifndef _GAME_H
#define _GAME_H

#include <vector>
#include "game/Player.h"
#include "map/Structure.h"

/**
 * @brief Klasse, die den Zustand des Spiels beinhaltet
 */
class Game {

private:
    /**
     * @brief Gesetzt, wenn gerade eine neue Struktur auf der Karte positioniert wird, welche.
     */
	StructureType addingStructure = StructureType::NO_STRUCTURE;
    
    /**
     * @brief Vektor von Spielern
     */
    std::vector<Player*> players;

public:
	Game();
	~Game();
    
    StructureType getAddingStructure() const {
        return addingStructure;
    }

    void setAddingStructure(StructureType addingStructure) {
        this->addingStructure = addingStructure;
    }
    
    /**
     * @brief Fügt einen neuen Spieler hinzu. Sinnvollerweise sollte das nur zu Beginns des Spiels gemacht werden.
     * Die Spieler werden vom Destruktor am Ende freigegeben
     * @param player Spieler
     */
    void addPlayer(Player* player) {
        players.push_back(player);
    }
    
    /**
     * @brief Liefert einen bestimmten Spieler zurück
     * @param playerIndex Index des Spielers.
     * @return Spieler
     */
    Player* getPlayer(int playerIndex) const {
        return players.at(playerIndex);
    }

};

#endif
