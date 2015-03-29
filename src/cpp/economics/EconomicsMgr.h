#ifndef _ECONOMICS_MGR_H
#define _ECONOMICS_MGR_H

#include <map>
#include <iostream>
#include "Context.h"
#include "map/Building.h"

struct Structure;

/**
 * @brief Hilfsstruktur, die das Ergebnis von @ref findBuildingToGetGoodsFrom() enthält.
 */
struct FindBuildingToGetGoodsFromResult {

    /**
     * @brief Gebäude, von dem wir waren holen sollten (oder `nullptr`, wenn grade nix zu holen is)
     */
    Building* building;

    /**
     * @brief berechnete Route zu dem Gebäude
     */
    Route route;

    /**
     * @brief GoodsSlot. Enthält die Info, welchen Warentyp wir dort holen sollen (`NO_GOODS`, wenn building leer ist),
     * wie hoch der Lagerbestand und die Lagerkapazität ist (um auswerten zu können, wie "gut" dieses Ergebnis ist)
     * @sa EconomicsMgr::findBuildingToGetGoodsFrom()
     */
    GoodsSlot goodsSlot;

    /**
     * @brief SDL_GetTicks-Wert, wann zuletzt eine Abholung der Waren bei diesem Gebäude stattgefunden hat
     */
    unsigned int lastGoodsCollections;

    /**
     * leerer Konstruktur, der ein "nix zu holen"-Ergebnis anlegt
     */
    FindBuildingToGetGoodsFromResult() {
        building = nullptr;
        goodsSlot.goodsType = GoodsType::NO_GOODS;
        lastGoodsCollections = 0;
    }

#ifdef DEBUG_ECONOMICS
    /**
     * @brief (nur zu Debugzwecken einkompiliert) Ausgabe-Operator, um diese Hilfsstruktur auszugeben
     * @param outputStream Ausgabestream, auf den geschrieben wird
     * @param result Objekt, was ausgegeben werden soll
     * @return Ausgabestream, sodass das Chaining funktioniert
     */
    friend std::ostream& operator<< (std::ostream& outputStream, FindBuildingToGetGoodsFromResult const& result) {
        const MapCoords& mapCoords = result.building->getMapCoords();

        return outputStream << "{ building = (" << mapCoords.x() << ", " << mapCoords.y() << "), routeLen = " <<
            result.route.size() << ", goods = ( goodsType = " << result.goodsSlot.goodsType <<
            ", inventory = " << result.goodsSlot.inventory << " of " << result.goodsSlot.capacity <<
            ", ratio = " << (result.goodsSlot.inventory / result.goodsSlot.capacity) <<
            " ), lastGoodsCollections = " << result.lastGoodsCollections << " }";
    }
#endif

};


/**
 * @brief Manager, der die Wirtschaft übernimmt
 */
class EconomicsMgr : public ContextAware {

public:
    EconomicsMgr(const Context* const context);
    ~EconomicsMgr();

    /**
     * @brief Aktualisiert eine Struktur. D.h. es werden Waren produziert, die zuständigen Männchen und Tierchen
     * bewegt und was sonst noch alles zum "Leben" der Struktur dazugehört.
     * @param structure Struktur, die aktualisiert werden soll
     */
    void update(Structure* structure);

private:
    /**
     * @brief Führt die Produktion für ein Gebäude durch. Es wird nur für jene vergangene Ticks produziert, für die
     * alle Eingabewaren da waren und auch nur solange das Ausgabewarenlager nicht voll ist.
     *
     * Algorithmus:
     * Es wird ermittelt, wie viel verbraucht würde, wenn in allen vergangenen Ticks die Voraussetzungen erfüllt sind.
     * Dann wird ermittelt, wie viele Ticks lang pro Slot die Voraussetzungen wirklich erfüllt sind. Mit dem Minimum
     * dieser Tick-Werte wird dann die Produktion durchgeführt.
     *
     * @param building Gebäude, für welches die Produktion durchgeführt werden soll
     */
    void updateProduction(Building* building);

    /**
     * @brief Aktualisiert den (TODO: die) Träger für ein Gebäude.
     * Der Träger wird losgeschickt, um passende Waren abzuholen bzw. wenn er schon unterwegs ist, wird er animiert,
     * fortbewegt und am Zielort die entsprechenden Operationen durchgeführt, um die Waren umzuladen.
     *
     * @param building Gebäude, für welches die Träger aktualisiert werden sollen
     */
    void updateCarrier(Building* building);

    /**
     * @brief Sucht für ein Gebäude einen GoodsSlot, in welchen ein Träger seine Waren laden soll.
     * Handelt es sich bei dem Gebäude um ein Lagergebäude, wird der passende Slot in der Kolonie zurückgegeben,
     * ansonsten wird der passende Slot `input` oder `input2` vom Gebäude zurückgegeben.
     *
     * @param building Gebäude, das das Ziel des Trägers ist
     * @param carrier Träger, der Waren hält und diese ausladen möchte
     * @return GoodsSlot, in welchen die Waren ausgeladen werden sollten
     */
    GoodsSlot* findGoodsSlotToUnloadTo(Building* building, Carrier* carrier);

    /**
     * @brief Sucht für ein angegebenes Gebäude das beste Gebäude, um Waren zu beziehen.
     *
     * "bestes" bedeutet hierbei:
     *  - Je voller das Lager (prozentual), desto besser der Treffer.
     *  - Haben zwei Gebäude gleichen Lagerstand, wird das Gebäude bevorzugt, von dem länger nicht abgeholt wurde
     *
     * @param building Gebäude, für welches wir eine Warenzulieferung suchen
     * @return Ergebnisstruktur, siehe Dokumentation dort
     */
    FindBuildingToGetGoodsFromResult findBuildingToGetGoodsFrom(Building* building);

};

#endif
