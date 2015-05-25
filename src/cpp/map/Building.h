#ifndef _BUILDING_H
#define _BUILDING_H

#include <gtest/gtest.h>
#include "economics/Carrier.h"
#include "game/ProductionSlots.h"
#include "map/Structure.h"


/**
 * @brief Structure, die ein Gebäude darstellt.
 */
class Building : public Structure {

    FRIEND_TEST(EconomicsMgrTest, updateCarrier);
    friend class EconomicsMgr; // EconomicsMgr soll zum Bewegen des Trägers einfach zugreifen können
    friend class GuiMap; // GuiMap soll zum Rendern des Trägers einfach zugreifen können

public:
    /**
     * @brief Produktionsslots des Gebäudes
     */
    ProductionSlots productionSlots;

    /**
     * @brief gibt an, wie viele Einwohner sich im Gebäude befinden.
     */
    unsigned char inhabitants;

    // TODO Aktuell hat jedes Gebäude bis zu einem Träger. Das muss später dynamisch nach Gebäudetyp sein.
    /**
     * @brief Referenz auf den Träger (owned durch Map.mapObjects), der zum Gebäude gehört
     */
    Carrier* carrier;

    /**
     * @brief SDL_GetTicks-Wert, wann zuletzt die Waren bei diesem Gebäude abgeholt wurden
     */
    uint32_t lastGoodsCollections;

public:
    Building() {
        carrier = nullptr;
        lastGoodsCollections = 0;
        inhabitants = 0;
    }

    virtual ~Building() {
    }

    virtual bool updateObject(const Context& context) override;
    
    /**
     * @brief Testet, ob eine bestimmte Kachel innerhalb des Einzugsgebiets des Gebäudes liegt
     * @param mapCoords Map-Koordinaten, die getestet wird
     * @return `true`, wenn die Kachel innerhalb des Einzugsgebiets liegt; sonst `false`
     */
    bool isInsideCatchmentArea(const MapCoords& mapCoords) const;
    
    /**
     * @brief Testet, ob ein bestimmtes Map-Objekt innerhalb des Einzugsgebiets des Gebäudes liegt. Dies ist immer dann
     * der Fall, wenn mindestens eine Kachel des Mapobjekts im Einzuggebiet liegt.
     *
     * @param otherMapObject Map-Objekt, was getestet wird
     * @return `true`, wenn das Map-Objekt innerhalb des Einzugsgebiets liegt; sonst `false`
     */
    bool isInsideCatchmentArea(const MapObjectFixed& otherMapObject) const;

    /**
     * @brief Prüft, ob das Gebäude ein Lagergebäude (Kontor oder Marketplatz), von welchem ALLE Waren abgeholt und
     * bezogen werden können.
     * @return `true` wenn Lagergebäude (Kontor oder Marketplatz), sonst `false`
     */
    bool isStorageBuilding() const {
        // TODO Flag über die Config steuern (= ob Einzugsbereich zum Baubereich zählt(?)))
        return (mapObjectType->name == "office1" || mapObjectType->name == "office2" ||
                mapObjectType->name == "marketplace");
    }

    /**
     * @brief Prüft, ob das Gebäude ein Haus ist
     * @return `true` wenn Haus, sonst `false`
     */
    bool isHouse() const {
        // TODO Flag über die Config steuern
        return (mapObjectType->name == "pioneers-house1" || mapObjectType->name == "pioneers-house2" ||
                mapObjectType->name == "pioneers-house3" || mapObjectType->name == "pioneers-house4" ||
                mapObjectType->name == "pioneers-house5" ||
                mapObjectType->name == "settlers-house1" || mapObjectType->name == "settlers-house2" ||
                mapObjectType->name == "settlers-house3" || mapObjectType->name == "settlers-house4" ||
                mapObjectType->name == "settlers-house5");
    }

private:
    /**
     * @brief Schickt einen (TODO: (weiteren)) Träger vom Gebäude los.
     * Wenn der (TODO: ein) Träger frei ist und ein Zielgebäude im Einzugsbereich ist, wird ein Träger losgeschickt,
     * um passende Waren abzuholen.
     *
     * @param context (Dependency)
     */
    void sendNewCarrier(const Context& context);

};

#endif
