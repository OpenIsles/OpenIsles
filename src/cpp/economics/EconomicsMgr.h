#ifndef _ECONOMICS_MGR_H
#define _ECONOMICS_MGR_H

#include "Context.h"
#include "map/Building.h"

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
     * @brief [Spielzeitpunkt](@ref gameTicks), wann zuletzt eine Abholung der Waren bei diesem Gebäude
     * stattgefunden hat
     */
    unsigned long lastGoodsCollections;

    /**
     * leerer Konstruktur, der ein "nix zu holen"-Ergebnis anlegt
     */
    FindBuildingToGetGoodsFromResult() {
        building = nullptr;
        goodsSlot.good = nullptr;
        lastGoodsCollections = 0;
    }

};


/**
 * @brief Manager, der die Wirtschaft übernimmt
 */
class EconomicsMgr : public ContextAware {

public:
    EconomicsMgr(const Context* const context);
    ~EconomicsMgr();

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
