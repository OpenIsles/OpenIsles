#ifndef _BUILDING_H
#define _BUILDING_H

#include "game/GoodsSlot.h"
#include "map/Structure.h"

/**
 * @brief Enthält die Slots für produzierte und zu verbrauchende Güter eines Gebäudes.
 */
typedef
struct ProductionSlots {

    /**
     * @brief produzierte Güter
     */
    GoodsSlot output;

    /**
     * @brief verbrauchte Güter
     */
    GoodsSlot input;

    /**
     * @brief weitere verbrauchte Güter, wenn das Gebäude aus zwei verschiedenen Gütern was herstellt
     */
    GoodsSlot input2;

} ProductionSlots;


/**
 * @brief Structure, die ein Gebäude darstellt.
 */
class Building : public Structure {

public:
    /**
     * @brief Produktionsslots des Gebäudes
     */
    ProductionSlots productionSlots;

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
