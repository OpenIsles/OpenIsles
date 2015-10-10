#include <list>
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "economics/InCatchmentAreaFinder.h"
#include "game/Game.h"


bool Building::updateObject(const Context& context) {
    context.economicsMgr->updateProduction(this);
    sendNewCarrier(context);
    return true;
}

void Building::sendNewCarrier(const Context& context) {
    // Alle Träger unterwegs?
    if (carriers.size() == mapObjectType->maxCarriers) {
        return;
    }

    // TODO Träger nicht alle gleichzeitig losschicken.
    // Jeder Träger soll eine Wartezeit haben, dass er nach der Lieferung erstmal nix tut.

    CatchmentAreaIterator catchmentAreaIterator(*this, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);
    InCatchmentAreaFinderResult result;

    // Lagergebäude holt alle verfügbaren Waren ab.
    if (isStorageBuilding()) {
        result = inCatchmentAreaFinder.findBuildingWithGoodsToStore();
        if (!result) {
            return;
        }

        addCarrierMapObject(context, result, "cart-without-cargo");
        return;
    }

    // Suche passende Input-Güter
    else if (productionSlots.input.isUsed()) {
        const Good* good1 = nullptr;
        const Good* good2 = nullptr;

        // Wir brauchen nur, wenn die Lager nicht voll sind
        if (productionSlots.input.getRemainingCapacity() >= 1) {
            good1 = productionSlots.input.good;
        }

        if (productionSlots.input2.isUsed()) {
            if (productionSlots.input2.getRemainingCapacity() >= 1) {
                good2 = productionSlots.input2.good;
            }
        }

        if (good1 == nullptr && good2 == nullptr) {
            return; // Alle Lager voll, wir brauchen nix.
        }

        std::list<const Good*> goods;

        // Wir brauchen nur ein Gut?
        if (good1 != nullptr && good2 == nullptr) {
            goods.push_back(good1);
        }
        else if (good1 == nullptr && good2 != nullptr) {
            goods.push_back(good2);
        }

        // Wenn wir zwei Güter brauchen, ermitteln, welches wir "dringender" brauchen.
        // Das ist jenes Gut, wo wir prozentual weniger auf Lager haben.
        else {
            if ((productionSlots.input.inventory / productionSlots.input.capacity) <
                (productionSlots.input2.inventory / productionSlots.input2.capacity)) {

                goods.push_back(good1);
                goods.push_back(good2);
            } else {
                goods.push_back(good2);
                goods.push_back(good1);
            }
        }


        result = inCatchmentAreaFinder.findBuildingWithGoods(goods);
        if (!result) {
            return;
        }

        addCarrierMapObject(context, result, "carrier");
        return;
    }

    else {
        // TODO weitere Fälle kommen später mal...

        // TODO Arzt/Feuerwehr: suchen unsichtbare Güter "Pest" und "Feuer" an den Wohngebäuden (vermutl. brennen auch andere Gebäude)
        // TODO Fischer/Schafe/Rinder: suchen Weideland bzw. Fischgründe
        // TODO Jägerhütte: Jäger sucht Wildtiere
        // TODO Steinmetz: sucht einen Steinbruch

        return;
    }
}

void Building::addCarrierMapObject(const Context& context,
                                   const InCatchmentAreaFinderResult& result,
                                   std::string graphicSetForCarrierAnimation) {

    const MapCoords& firstHopOnRoute = result.route.front();

    Carrier* carrier = new Carrier(this, result.route, result.good, true);
    carrier->setLastUpdateTicks(context.game->getTicks());
    carrier->setMapCoords((DoubleMapCoords) firstHopOnRoute);
    carrier->updateCurrentMovingDirection();
    carrier->setAnimations(*context.graphicsMgr->getGraphicSet(graphicSetForCarrierAnimation));

    context.game->getMap()->addMapObject(carrier);
    carriers.insert(carrier);

    // Slot markieren, dass nicht ein zweiter Träger hinläuft.
    // Zu einem Lagergebäude dürfen natürlich mehrere hinlaufen und sich bedienen.
    Building* targetBuilding = dynamic_cast<Building*>(result.mapObject);
    if (targetBuilding != nullptr && !targetBuilding->isStorageBuilding()) {
        targetBuilding->productionSlots.output.markedForPickup = true;
    }
}