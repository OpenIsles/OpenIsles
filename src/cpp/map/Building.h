#ifndef _BUILDING_H
#define _BUILDING_H

#include <gtest/gtest.h>
#include "economics/Carrier.h"
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

    FRIEND_TEST(EconomicsMgrTest, updateCarrier);
    friend class EconomicsMgr; // EconomicsMgr soll zum Bewegen des Trägers einfach zugreifen können
    friend class GuiMap; // GuiMap soll zum Rendern des Trägers einfach zugreifen können

private:
    // TODO Aktuell hat jedes Gebäude bis zu einem Träger. Das muss später dynamisch nach Gebäudetyp sein.
    /**
     * @brief Träger, der zum Gebäude gehört
     */
    Carrier* carrier;

    /**
     * @brief SDL_GetTicks-Wert, wann zuletzt die Waren bei diesem Gebäude abgeholt wurden
     */
    uint32_t lastGoodsCollections;

public:
    /**
     * @brief Produktionsslots des Gebäudes
     */
    ProductionSlots productionSlots;

    /**
     * @brief gibt an, wie viele Einwohner sich im Gebäude befinden.
     */
    unsigned char inhabitants;

public:
    Building() {
        carrier = nullptr;
        lastGoodsCollections = 0;
        inhabitants = 0;
    }

    virtual ~Building() {
        if (carrier != nullptr) {
            delete carrier;
        }
    }
    
    /**
     * @brief Testet, ob eine bestimmte Kachel innerhalb des Einzugsgebiets des Gebäudes liegt
     * @param configMgr Dependency
     * @param mapCoords Map-Koordinaten, die getestet wird
     * @return `true`, wenn die Kachel innerhalb des Einzugsgebiets liegt; sonst `false`
     */
    bool isInsideCatchmentArea(const ConfigMgr* configMgr, const MapCoords& mapCoords) const;
    
    /**
     * @brief Testet, ob ein bestimmtes Map-Objekt innerhalb des Einzugsgebiets des Gebäudes liegt. Dies ist immer dann
     * der Fall, wenn mindestens eine Kachel des Mapobjekts im Einzuggebiet liegt.
     *
     * @param configMgr Dependency
     * @param otherMapObject Map-Objekt, was getestet wird
     * @return `true`, wenn das Map-Objekt innerhalb des Einzugsgebiets liegt; sonst `false`
     */
    bool isInsideCatchmentArea(const ConfigMgr* configMgr, const MapObjectFixed& otherMapObject) const;

    /**
     * @brief Prüft, ob das Gebäude ein Lagergebäude (Kontor oder Marketplatz), von welchem ALLE Waren abgeholt und
     * bezogen werden können.
     * @return `true` wenn Lagergebäude (Kontor oder Marketplatz), sonst `false`
     */
    bool isStorageBuilding() const {
        return (mapObjectType == OFFICE1 || mapObjectType == OFFICE2 || mapObjectType == MARKETPLACE);
    }

    /**
     * @brief Prüft, ob das Gebäude ein Haus ist
     * @return `true` wenn Haus, sonst `false`
     */
    bool isHouse() const {
        return (mapObjectType == PIONEERS_HOUSE1 || mapObjectType == PIONEERS_HOUSE2 ||
                mapObjectType == PIONEERS_HOUSE3 || mapObjectType == PIONEERS_HOUSE4 ||
                mapObjectType == PIONEERS_HOUSE5 ||
                mapObjectType == SETTLERS_HOUSE1 || mapObjectType == SETTLERS_HOUSE2 ||
                mapObjectType == SETTLERS_HOUSE3 || mapObjectType == SETTLERS_HOUSE4);
    }

};

#endif
