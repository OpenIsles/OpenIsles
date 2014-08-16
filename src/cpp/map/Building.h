#ifndef _BUILDING_H
#define _BUILDING_H

#include "Structure.h"

/**
 * @brief Structure, die ein Gebäude darstellt.
 */
class Building : public Structure {     

public:
    Building() {}
    virtual ~Building() {}
    
    /**
     * @brief Testet, ob eine bestimmte Kachel innerhalb des Einzugsgebiets des Gebäudes liegt
     * @param mapX X-Map-Koordinate, die getestet wird
     * @param mapY Y-Map-Koordinate, die getestet wird
     * @return true, wenn die Kachel innerhalb des Einzugsgebiets liegt; sonst false
     */
    bool isInsideCatchmentArea(int mapX, int mapY) const;
    
    /**
     * @brief Testet, ob ein bestimmtes Map-Objekt innerhalb des Einzugsgebiets des Gebäudes liegt. Dies ist immer dann
     * der Fall, wenn mindestens eine Kachel des Mapobjekts im Einzuggebiet liegt.
     * @param mapObject Map-Objekt, was getestet wird
     * @return true, wenn das Map-Objekt innerhalb des Einzugsgebiets liegt; sonst false
     */
    bool isInsideCatchmentArea(MapObject* mapObject) const;
    
	/**
	 * @brief Callback, der sich um einen Mausklick auf das Gebäude kümmert
	 * @param mouseXInBuilding X-Koordinate innerhalb des Gebäude, wo geklickt wurde
	 * @param mouseYInBuilding Y-Koordinate innerhalb des Gebäude, wo geklickt wurde
	 */
	void onClick(int mouseXInBuilding, int mouseYInBuilding);
};

#endif
