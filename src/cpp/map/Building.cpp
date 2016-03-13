#include <cassert>
#include <list>
#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "economics/InCatchmentAreaFinder.h"
#include "game/Game.h"


bool Building::updateObject(const Context& context) {
    context.economicsMgr->updateProduction(this);
    sendNewCarrier(context);
    updateInhabitants(context);
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
    InCatchmentAreaFinder inCatchmentAreaFinder(context, &catchmentAreaIterator);
    InCatchmentAreaFinderResult result;

    // Lagergebäude holt alle verfügbaren Waren ab.
    if (isStorageBuilding()) {
        result = inCatchmentAreaFinder.findBuildingWithGoodsToStore();
        if (!result) {
            return;
        }

        const MapObjectType* carrierType = mapObjectType->carrier.mapObjectType;
        EightDirectionsAnimation animations =
            context.graphicsMgr->getGraphicSet("cart-without-cargo")->getEightDirectionsAnimation();
        addCarrierMapObject(context, result, carrierType, animations);
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
            if (((double) productionSlots.input.inventory / (double) productionSlots.input.capacity) <
                ((double) productionSlots.input2.inventory / (double) productionSlots.input2.capacity)) {

                goods.push_back(good1);
                goods.push_back(good2);
            } else {
                goods.push_back(good2);
                goods.push_back(good1);
            }
        }

        /* Fallunterscheidung:
         * Der Code für unsichtbare Güter ist ein wenig anders.
         * Der Fall, dass zwei Güter angefordert werden, ein sichtbares und ein unsichtbares, gibt es nicht!
         */
        assert(goods.size() != 2 ||
               (!(*goods.cbegin())->invisible && !(*(++goods.cbegin()))->invisible)
        );

        const Good* firstGood = *goods.cbegin();
        if (firstGood->invisible) {
            // TODO Träger über Config steuern: Animations
            // TODO Rinderfarm sollte später Rinder haben, keine Schafe ;-)
            assert(mapObjectType->name == "sheep-farm" || mapObjectType->name == "cattle-farm");

            result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(firstGood);
            if (!result) {
                return;
            }

            const MapObjectType* carrierType = mapObjectType->carrier.mapObjectType;
            EightDirectionsAnimation animations; // TODO Träger über Config steuern: Animations
            if (mapObjectType->name == "sheep-farm") {
                animations = context.graphicsMgr->getGraphicSet("sheep0")->getEightDirectionsAnimation("walking");
            } else if (mapObjectType->name == "cattle-farm") {
                animations = context.graphicsMgr->getGraphicSet("cattle")->getEightDirectionsAnimation("walking");
            } else {
                assert(false);
            }
            addCarrierMapObject(context, result.route, firstGood, carrierType, animations);

            // Slot markieren, dass nicht ein zweiter Träger hinläuft.
            context.game->getMap()->getMapTileAt(result.mapCoords)->harvestBusy = true;

            // TODO weitere Fälle kommen später mal...

            // TODO Arzt/Feuerwehr: suchen unsichtbare Güter "Pest" und "Feuer" an den Wohngebäuden (vermutl. brennen auch andere Gebäude)
            // TODO Fischer: suchen Fischgründe
            // TODO Jägerhütte: Jäger sucht Wildtiere
            // TODO Steinmetz: sucht einen Steinbruch
        }
        else {
            result = inCatchmentAreaFinder.findBuildingWithGoods(goods);
            if (!result) {
                return;
            }

            const MapObjectType* carrierType = mapObjectType->carrier.mapObjectType;
            EightDirectionsAnimation animations =
                context.graphicsMgr->getGraphicSet("carrier")->getEightDirectionsAnimation();
            addCarrierMapObject(context, result, carrierType, animations);
        }

        return;
    }
}

void Building::addCarrierMapObject(const Context& context,
                                   const InCatchmentAreaFinderResult& result,
                                   const MapObjectType* carrierType,
                                   const EightDirectionsAnimation& animations) {

    addCarrierMapObject(context, result.route, result.good, carrierType, animations);

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
                                   const EightDirectionsAnimation& animations) {

    const unsigned long gameTicks = context.game->getTicks();

    Carrier* carrier = (Carrier*) MapObject::instantiate(carrierType);
    carrier->initRoute(this, route, good, carrierType->carrier.capacity, true, animations, gameTicks);

    context.game->getMap()->addMapObject(carrier);
    carriers.insert(carrier);

    // Delay bis zum nächsten Träger einstellen
    nextCarrierMinTicks = gameTicks + Building::CARRIER_DELAY;
}

void Building::updateInhabitants(const Context& context) {
    // Bin ich ein Haus?
    if (!isHouse()) {
        return;
    }

    // Haus voll?
    const PopulationTier* populationTier = mapObjectType->populationTier;
    if (inhabitants >= populationTier->maxPopulationPerHouse) {
        return;
    }

    // Laune berücksichtigen. Neutral = keine Veränderung
    if (colony->populationTiers[populationTier].populationSatisfaction == PopulationSatisfaction::NEUTRAL) {
        return;
    }

    if (colony->populationTiers[populationTier].populationSatisfaction == PopulationSatisfaction::BAD ||
        colony->populationTiers[populationTier].populationSatisfaction == PopulationSatisfaction::WORST) {

        // TODO mies gelaunte Häuser
        Log::debug("Houses are not satisfied. TODO decrease inhabitants");
        return;
    }

    // In den ersten 2 Minuten kein Bevölkerungswachstum
    if (context.game->getTicks() - createdTicks < 125*TICKS_PER_SECOND) {
        return;
    }

    // Nicht alle Gebäude auf einmal. Maximal alle 500ms einer
    if (context.game->getTicks() - colony->populationTiers[populationTier].lastIncreaseTicks < 500) {
        return;
    }

    colony->populationTiers[populationTier].lastIncreaseTicks = context.game->getTicks();
    context.game->addInhabitantsToBuilding(this, 1);
    // TODO Siedler und höhere Bevölkerungsgruppen nehmen nicht um 1, sondern mehr zu
}