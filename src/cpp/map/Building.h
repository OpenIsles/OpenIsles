#ifndef _BUILDING_H
#define _BUILDING_H

#ifdef TESTS_ENABLED
#include <gtest/gtest.h>
#endif

#include <string>
#include <unordered_set>
#include "economics/Carrier.h"
#include "economics/InCatchmentAreaFinder.h"
#include "game/ProductionSlots.h"
#include "map/Structure.h"


/**
 * @brief Structure, die ein Gebäude darstellt.
 */
class Building : public Structure {

#ifdef TESTS_ENABLED
    FRIEND_TEST(EconomicsMgrTest, updateCarrier);
#endif
    friend class EconomicsMgr; // EconomicsMgr soll zum Bewegen des Trägers einfach zugreifen können
    friend class GuiMap; // GuiMap soll zum Rendern des Trägers einfach zugreifen können

public:
    /**
     * @brief Minimale Verzögerung in [Millisekunden](@ref gameTicks) zwischen Gebäude-Bau und dem Losschicken
     * des ersten Trägers
     */
    static constexpr unsigned int CARRIER_INITIAL_DELAY = 2000;

    /**
     * @brief Minimale Verzögerung in [Millisekunden](@ref gameTicks) zwischen dem Losschicken von zwei Trägern.
     */
    static constexpr unsigned int CARRIER_DELAY = 10000;

public:
    /**
     * @brief Produktionsslots des Gebäudes
     */
    ProductionSlots productionSlots;

    /**
     * @brief gibt an, wie viele Einwohner sich im Gebäude befinden.
     */
    unsigned char inhabitants;

    /**
     * @brief Referenzen auf die Träger (owned durch Map.mapObjects), die zum Gebäude gehören
     */
    std::unordered_set<Carrier*> carriers;

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann frühestens ein neuer Träger losgeschickt werden kann
     * Wir nutzen das, um nicht alle Träger gleichzeitig loszuschicken.
     */
    unsigned long nextCarrierMinTicks;

    /**
     * @brief [Zeitpunkt](@ref gameTicks), wann zuletzt die Waren bei diesem Gebäude abgeholt wurden
     */
    unsigned long lastGoodsCollections;

public:
    Building() {
        lastGoodsCollections = 0;
        inhabitants = 0;
        nextCarrierMinTicks = 0;
    }

    virtual ~Building() {
    }

    virtual bool updateObject(const Context& context) override;

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
     * @brief Schickt einen weiteren Träger vom Gebäude los.
     * Wenn ein Träger frei ist und ein Zielgebäude im Einzugsbereich ist, wird ein Träger losgeschickt,
     * um passende Waren abzuholen.
     *
     * @param context (Dependency)
     */
    void sendNewCarrier(const Context& context);

    /**
     * @brief Legt einen Träger auf der Map an und zuweist ihm dem Gebäude zu.
     *
     * @param context (Dependency)
     * @param result Ergebnis-Struktur, die zu verwendende Route und zu holendes Gut enthält
     * @param carrierType `MapObjectType` des zu erstellenden Trägers
     * @param animations Animationen, die für den Träger benutzt werden sollen
     */
    void addCarrierMapObject(const Context& context,
                             const InCatchmentAreaFinderResult& result,
                             const MapObjectType* carrierType,
                             const EightDirectionsAnimation& animations);

    /**
     * @brief Legt einen Träger auf der Map an und zuweist ihm dem Gebäude zu.
     *
     * @param context (Dependency)
     * @param route zu verwendende Route
     * @param good zu holendes Gut
     * @param carrierType `MapObjectType` des zu erstellenden Trägers
     * @param animations Animationen, die für den Träger benutzt werden sollen
     */
    void addCarrierMapObject(const Context& context,
                             const Route& route, const Good* good,
                             const MapObjectType* carrierType,
                             const EightDirectionsAnimation& animations);

};

#endif
