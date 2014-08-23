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
    
    /**
     * @brief Zeiger auf den Spieler, den dieser Client kontrolliert
     */
    Player* currentPlayer = nullptr;

public:
	Game();
	~Game();
    
    StructureType getAddingStructure() const {
        return addingStructure;
    }
    
    /**
     * @return true, wenn wir grade ein Gebäude positionieren, sonst false
     */
    bool isAddingStructure() const {
        return (addingStructure != StructureType::NO_STRUCTURE);
    }

    /**
     * @brief Wechselt in den "Gebäude platzieren"-Modus, bzw. wenn wir schon drin waren, wechselt das zu platzierende
     * Gebäude
     * @param addingStructure
     */
    void startAddingStructure(StructureType addingStructure) {
        this->addingStructure = addingStructure;
    }
    
    /**
     * @brief Beendet den "Gebäude platzieren"-Modus
     */
    void endAddingStructure() {
        this->addingStructure = StructureType::NO_STRUCTURE;
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

    /**
     * @brief Setzt den aktuellen Spieler. Das muss man wohl nur ein einziges Mal machen.
     * @param currentPlayer Spieler, der als aktueller Spieler gesetzt werden soll.
     */
    void setCurrentPlayer(Player* currentPlayer) {
        this->currentPlayer = currentPlayer;
    }
    
    /**
     * @brief Liefert den aktueller Spieler zurück
     * @return aktueller Spieler
     */
    Player* getCurrentPlayer() const {
        return currentPlayer;
    }

};

#endif
