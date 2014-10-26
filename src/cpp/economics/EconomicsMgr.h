#ifndef _ECONOMICS_MGR_H
#define _ECONOMICS_MGR_H

#include <map>
#include "map/Building.h"

struct Structure;

/**
 * @brief Hilfsstruktur, die das Ergebnis von @ref findBuildingToGetGoodsFrom() enthält.
 */
struct FindBuildingToGetGoodsFromResult {

    /**
     * @brief Gebäude, von dem wir waren holen sollten (oder nullptr, wenn grade nix zu holen is)
     */
    Building* building;

    /**
     * @brief berechnete Route zu dem Gebäude (oder null, wenn building leer ist)
     */
    Route* route;

    /**
     * @brief Warentyp, den wir dort holen sollen (NO_GOODS, wenn building leer ist)
     */
    GoodsType goods;

    /**
     * leerer Konstruktur, der ein "nix zu holen"-Ergebnis anlegt
     */
    FindBuildingToGetGoodsFromResult() {
        building = nullptr;
        route = nullptr;
        goods = GoodsType::NO_GOODS;
    }

};


/**
 * @brief Manager, der die Wirtschaft übernimmt
 */
class EconomicsMgr {

public:
    EconomicsMgr();
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
     * @brief Sucht für ein angegebenes Gebäude das beste Gebäude, um Waren von diesen zu beziehen
     * "bestes" bedeutet hierbei, dass es Güter hat, die wir brauchen und entsprechender Lagerbestand vorhanden ist.
     * Gebäude mit höherem Lagerbestand werden bevorzugt, dass wir nicht ständig vom selben nahen Gebäude 1t abholen,
     * während ein paar Kacheln weiter die Lager überlaufen.
     * @param building Gebäude, für welches wir eine Warenzulieferung suchen
     * @return Ergebnisstruktur, siehe Dokumentation dort
     */
    FindBuildingToGetGoodsFromResult findBuildingToGetGoodsFrom(Building* building);

};

#endif
