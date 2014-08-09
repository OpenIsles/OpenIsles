#ifndef _GAME_H
#define _GAME_H

enum StructureType : unsigned char;

/**
 * @brief Klasse, die den Zustand des Spiels beinhaltet
 */
class Game {

private:
    /**
     * @brief Gesetzt, wenn gerade eine neue Struktur auf der Karte positioniert wird, welche.
     */
	StructureType addingStructure = NO_STRUCTURE;

public:
	Game();
	~Game();
    
    StructureType getAddingStructure() const {
        return addingStructure;
    }

    void setAddingStructure(StructureType addingStructure) {
        this->addingStructure = addingStructure;
    }

};

#endif
