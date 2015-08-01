#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"


bool Building::updateObject(const Context& context) {
    context.economicsMgr->updateProduction(this);
    sendNewCarrier(context);
    return true;
}

void Building::sendNewCarrier(const Context& context) {
    // Weder Lagergebäude (was Marktkarren hat), noch Produktionsgebäude (was Eingabegüter hat)? Nix zu tun
    if (!productionSlots.input.isUsed() && !isStorageBuilding()) {
        return;
    }

    // Ist der Träger zu Hause? Gucken, wo was zu holen is und Abholung einleiten
    if (carrier == nullptr) {
        FindBuildingToGetGoodsFromResult result = context.economicsMgr->findBuildingToGetGoodsFrom(this);
        if (result.building == nullptr) {
            return; // nix zu tun
        }

        // Träger anlegen und zuweisen
        const MapCoords& firstHopOnRoute = result.route.front();

        carrier = new Carrier(this, result.route, result.goodsSlot.good, true);
        carrier->setMapCoords((DoubleMapCoords) firstHopOnRoute);
        carrier->updateCurrentMovingDirection();
        carrier->setAnimations(
            *context.graphicsMgr->getGraphicSet(isStorageBuilding() ? "cart-without-cargo" : "carrier"));

        context.game->getMap()->addMapObject(carrier);

        // Slot markieren, dass nicht ein zweiter Träger hinläuft.
        // Zu einem Lagergebäude dürfen natürlich mehrere hinlaufen und sich bedienen.
        if (!result.building->isStorageBuilding()) {
            result.building->productionSlots.output.markedForPickup = true;
        }
    }
}
