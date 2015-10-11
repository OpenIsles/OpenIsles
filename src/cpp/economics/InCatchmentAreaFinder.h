#ifndef _IN_CATCHMENT_AREA_FINDER_H
#define _IN_CATCHMENT_AREA_FINDER_H

#include <list>
#include "Context.h"
#include "config/Good.h"
#include "map/coords/MapCoords.h"
#include "map/MapObject.h"
#include "pathfinding/AStar.h"
#include "utils/CatchmentAreaIterator.h"

/**
 * @brief Ergebnisstruktur für die Methoden von InCatchmentAreaFinder.
 *
 * Genaue Beschreibung, welche Felder wie befüllt sind, sind im Kommentar der
 * jeweiligen Methode von InCatchmentAreaFinder zu finden.
 */
struct InCatchmentAreaFinderResult {

    /**
     * @brief `false`, um anzeigen, dass nix gefunden wurde.
     */
    bool foundSomething = false;

    MapCoords mapCoords;
    MapObject* mapObject = nullptr;
    Route route;
    const Good* good = nullptr;

    /**
     * @brief Cast-Operator zu `bool`, der genau dann `true` zurückgibt, wenn ein Ergebnis gefunden wurde.
     */
    operator bool() const {
        return foundSomething;
    }
};


/**
 * @brief Sucht innerhalb eines Einzugsbereichs bestimmte Sachen.
 */
class InCatchmentAreaFinder : public ContextAware {

private:
    /**
     * @brief Einzugsbereich, in dem gesucht werden soll
     */
    CatchmentAreaIterator* catchmentAreaIterator;

public:
    InCatchmentAreaFinder(const Context* const context, CatchmentAreaIterator* catchmentAreaIterator);

    /**
     * @brief Sucht im Einzugsbereich nach dem "besten" Gebäude, dass abholfertige Güter für ein Lagergebäude zur
     * Verfügung stellt.
     *
     * "bestes" bedeutet hierbei:
     *  - Je voller das Lager (prozentual), desto besser der Treffer.
     *  - Haben zwei Gebäude gleichen Lagerstand, wird das Gebäude bevorzugt, von dem länger nicht abgeholt wurde
     *
     * Gebäude, die Rohstoffe herstellen, werden nur berücksichtigt, wenn das Lager randvoll ist.
     *
     * @return Ergebnisstruktur.
     * `mapObject` enthält das Zielgebäude, `good` welches Gut abzuholen ist und `route` die Route zum Zielgebäude.
     */
    InCatchmentAreaFinderResult findBuildingWithGoodsToStore();

    /**
     * @brief Sucht im Einzugsbereich nach dem "besten" Gebäude, dass bestimmte Güter zur Verfügung stellt.
     *
     * "bestes" bedeutet hierbei:
     *  - Grundsätzlich wird die Priorisierung in `goods` eingehalten. Das ist strikt notwendig, weil sonst
     *    für Gebäude, die zwei Input-Güter brauchen u.U. immer nur Gut 1 geholt wird, niemals Gut 2.
     *  - Je voller das Lager (prozentual), desto besser der Treffer.
     *  - Haben zwei Gebäude gleichen Lagerstand, wird das Gebäude bevorzugt, von dem länger nicht abgeholt wurde
     *
     * @param goods benötigte Güter (nach Priorität geordnet! höhere Prio zuerst)
     * @return Ergebnisstruktur.
     * `mapObject` enthält das Zielgebäude, `good` welches Gut abzuholen ist und `route` die Route zum Zielgebäude.
     */
    InCatchmentAreaFinderResult findBuildingWithGoods(std::list<const Good*> goods);

    /**
     * @brief Sucht im Einzugsbereich nach der "besten" Map-Kachel, die ein bestimmtes unsichtbares Gut
     * zur Verfügung stellt.
     *
     * "bestes" bedeutet hierbei:
     *  - Grundsätzlich kommen nur Kacheln in Frage, die seit einer bestimmten Zeit nicht abgeerntet wurden.
     *  - Ansonsten wird die Kachel genommen, die am längsten nicht abgeerntet wurde.
     *
     * @param invisibleGood benötiges unsichtbares Gut
     * @return Ergebnisstruktur.
     * `mapCoords` enthält die Zielkachel und `route` die Route zur Zielkachel.
     */
    InCatchmentAreaFinderResult findMapTileWithInvisibleGood(const Good* invisibleGood);

private:
    /**
     * @brief Interner Helper
     *
     * @param isStorageBuilding `true`, wenn wir für ein Lagergebäude Waren suchen, d.&nbsp;h. alle abholfertigen
     *                           Waren akzeptieren
     * @param goods gesuchte Güter (oder `nullptr`, wenn `isStorageBuilding == true`).
     * @return Ergebnisstruktur
     *
     * @sa findBuildingWithGoodsToStore()
     * @sa findBuildingWithGoods(std::list<const Good*> goods)
     */
    InCatchmentAreaFinderResult findBuildingWithGoods(bool isStorageBuilding, const std::list<const Good*>* goods);

};

/* TODO: weitere Methoden bauen
 * - Suche MapTile mit bestimmter Gut (Schaffarm: Route für ein Schaf zum Weideland)
 * - Suche bestimmtes Harvestable (Forsthaus: Förster sucht Route zu einem abholzbaren Wald)
 * - Suche MapObjectMoving mit bestimmtem Gut (Jäger sucht Wild)
 */

#endif