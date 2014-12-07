#ifndef _GAME_H
#define _GAME_H

#include <map>
#include <vector>
#include "Context.h"

class Building;
class Colony;
class Isle;
class Map;
class MapObject;
class Player;

/**
 * @brief Klasse, die den Zustand des Spiels beinhaltet
 */
class Game : public ContextAware {

private:
    /**
     * @brief die Karte. Enthält die Inseln und alles was drauf is
     */
    Map* map;
    
    /**
     * @brief Vektor von Spielern
     */
    std::vector<Player*> players;
    
    /**
     * @brief Zeiger auf den Spieler, den dieser Client kontrolliert
     */
    Player* currentPlayer = nullptr;
    
    /**
     * @brief Siedlungen im Spiel. Jeder Spieler kann auf jeder Insel nur maximal eine Siedlung haben.
     */
    std::map<std::pair<Player*, Isle*>, Colony*> colonies;

    /**
     * @brief Spielgeschwindigkeit in Vielfachen der Normalgeschwindigkeit
     */
    double speed;

public:
	Game(const Context* const context);
	~Game();

    Map* getMap() const {
        return map;
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
    
    /**
     * Gründet eine neue Siedlung
     * 
     * @param player Spieler, der die Siedlung gründet
     * @param isle Insel, auf der die Siedlung gegründet wird
     * @return Zeiger auf die neu angelegte Siedlung
     */
    Colony* foundNewColony(Player* player, Isle* isle);
    
    /**
     * Liefert die Siedlung eines Spielers auf einer Insel zurück
     * 
     * @param player Spieler
     * @param isle Insel
     * @return Colony oder nullptr, wenn keine Siedlung da ist
     */
    Colony* getColony(Player* player, Isle* isle);

    /**
     * Liefert die Siedlung zurück, in der ein bestimmtes MapObject sich befindet.
     *
     * @param mapObject MapObject
     * @return Colony oder nullptr, wenn keine Siedlung da ist
     */
    Colony* getColony(const MapObject* mapObject);

    /**
     * @brief Liefert die Spielgeschwindigkeit zurück.
     * @return Spielgeschwindigkeit in Vielfachen der Normalgeschwindigkeit
     */
    double getSpeed() const {
        return speed;
    }

    /**
     * @brief Ändert die Spielgeschwindigkeit
     * @param speed neue Spielgeschwindigkeit
     */
    void setSpeed(double speed) {
        this->speed = speed;
    }

    /**
     * @brief Ändert die Einwohner innerhalb eines Gebäudes. Positive Werte fügen Bewohner hinzu, negative nehmen
     * welche weg.
     *
     * Diese Methode aktualisiert gleich die Einwohnerzahl der Kolonie. Es sollte immer diese Methode benutzt werden,
     * um Einwohner zu ändern.
     *
     * @param building Gebäude, dessen Einwohner geändert werden sollen
     * @param amount Bevölkerung, die hinzukommen soll
     */
    void addInhabitantsToBuilding(Building* building, char amount);

};

#endif
