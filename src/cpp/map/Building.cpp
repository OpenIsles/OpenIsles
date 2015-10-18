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

    // Wartezeit einhalten, damit nicht alle Träger gleichzeitig losstürmen
    if (context.game->getTicks() < nextCarrierMinTicks) {
        return;
    }

    CatchmentAreaIterator catchmentAreaIterator(*this, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);
    InCatchmentAreaFinderResult result;

    // Lagergebäude holt alle verfügbaren Waren ab.
    if (isStorageBuilding()) {
        result = inCatchmentAreaFinder.findBuildingWithGoodsToStore();
        if (!result) {
            return;
        }

        const MapObjectType* carrierType = mapObjectType->carrierType;
        addCarrierMapObject(context, result, carrierType, "cart-without-cargo");
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

        const MapObjectType* carrierType = mapObjectType->carrierType;
        addCarrierMapObject(context, result, carrierType, "carrier");
        return;
    }

    else {
        // TODO Träger über Config steuern
        // TODO Rinderfarm sollte später Rinder haben, keine Schafe ;-)
        if (mapObjectType->name == "sheep-farm" || mapObjectType->name == "cattle-farm") {
            const Good* goodGrassland = context.configMgr->getGood("grassland");

            result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(goodGrassland);
            if (!result) {
                return;
            }

            const MapObjectType* carrierType = mapObjectType->carrierType;
            addCarrierMapObject(context, result.route, goodGrassland, carrierType, "sheep");

            // Slot markieren, dass nicht ein zweiter Träger hinläuft.
            context.game->getMap()->getMapTileAt(result.mapCoords)->harvestBusy = true;
        }

        // TODO weitere Fälle kommen später mal...

        // TODO Arzt/Feuerwehr: suchen unsichtbare Güter "Pest" und "Feuer" an den Wohngebäuden (vermutl. brennen auch andere Gebäude)
        // TODO Fischer: suchen Fischgründe
        // TODO Jägerhütte: Jäger sucht Wildtiere
        // TODO Steinmetz: sucht einen Steinbruch

        return;
    }
}

void Building::addCarrierMapObject(const Context& context,
                                   const InCatchmentAreaFinderResult& result,
                                   const MapObjectType* carrierType,
                                   std::string graphicSetForCarrierAnimation) {

    addCarrierMapObject(context, result.route, result.good, carrierType, graphicSetForCarrierAnimation);

    // Slot markieren, dass nicht ein zweiter Träger hinläuft.
    // Zu einem Lagergebäude dürfen natürlich mehrere hinlaufen und sich bedienen.
    Building* targetBuilding = dynamic_cast<Building*>(result.mapObject);
    if (targetBuilding != nullptr && !targetBuilding->isStorageBuilding()) {
        targetBuilding->productionSlots.output.markedForPickup = true;
    }
}

void Building::addCarrierMapObject(const Context& context,
                                   const Route& route, const Good* good,
                                   const MapObjectType* carrierType,
                                   std::string graphicSetForCarrierAnimation) {

    const MapCoords& firstHopOnRoute = route.front();
    const unsigned long gameTicks = context.game->getTicks();

    Carrier* carrier = (Carrier*) MapObject::instantiate(carrierType);
    carrier->initRoute(this, route, good, true);
    carrier->setLastUpdateTicks(gameTicks);
    carrier->setMapCoords((DoubleMapCoords) firstHopOnRoute);
    carrier->updateCurrentMovingDirection();
    carrier->setAnimations(*context.graphicsMgr->getGraphicSet(graphicSetForCarrierAnimation));

    context.game->getMap()->addMapObject(carrier);
    carriers.insert(carrier);

    // Delay bis zum nächsten Träger einstellen
    nextCarrierMinTicks = gameTicks + Building::CARRIER_DELAY;
}