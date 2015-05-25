#include "config/ConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Game.h"


bool Building::isInsideCatchmentArea(const MapCoords& mapCoords) const {
    // Gebäude hat keinen Einzugsbereich?
    if (!mapObjectType->catchmentArea) {
        return false;
    }
    const RectangleData<char>& catchmentArea = *mapObjectType->catchmentArea;

    // Koordinaten innerhalb von mapObjectConfig.catchmentArea.data ermitteln
    int x, y;
    if (view == Direction::SOUTH) {
        x = (mapCoords.x() - this->mapCoords.x()) + ((catchmentArea.width - this->mapWidth) / 2);
        y = (mapCoords.y() - this->mapCoords.y()) + ((catchmentArea.height - this->mapHeight) / 2);
        return (catchmentArea.getData(x, y, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen

    } else if (view == Direction::EAST ) {
        x = (mapCoords.x() - this->mapCoords.x()) + ((catchmentArea.height - this->mapWidth) / 2);
        y = (mapCoords.y() - this->mapCoords.y()) + ((catchmentArea.width - this->mapHeight) / 2);
        return (catchmentArea.getData(y, (catchmentArea.height - 1) - x, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen

    } else if (view == Direction::NORTH) {
        x = (mapCoords.x() - this->mapCoords.x()) + ((catchmentArea.width - this->mapWidth) / 2);
        y = (mapCoords.y() - this->mapCoords.y()) + ((catchmentArea.height - this->mapHeight) / 2);
        return (catchmentArea.getData((catchmentArea.width - 1) - x, (catchmentArea.height - 1) - y, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen

    } else if (view == Direction::WEST) {
        x = (mapCoords.x() - this->mapCoords.x()) + ((catchmentArea.height - this->mapWidth) / 2);
        y = (mapCoords.y() - this->mapCoords.y()) + ((catchmentArea.width - this->mapHeight) / 2);
        return (catchmentArea.getData((catchmentArea.width - 1) - y, x, '0') == '1'); // TODO String-'1' durch echte 1 ersetzen, wenn wir das über die Config einlesen
    }
    else {
        assert(false);
        return false;
    }
}

bool Building::isInsideCatchmentArea(const MapObjectFixed& otherMapObject) const {
    const MapCoords& otherMapObjectCoords = otherMapObject.getMapCoords();
    int otherMapObjectWidth = otherMapObject.getMapWidth();
    int otherMapObjectHeight = otherMapObject.getMapHeight();

    for (int mapY = otherMapObjectCoords.y(); mapY < otherMapObjectCoords.y() + otherMapObjectHeight; mapY++) {
        for (int mapX = otherMapObjectCoords.x(); mapX < otherMapObjectCoords.x() + otherMapObjectWidth; mapX++) {
            if (isInsideCatchmentArea(MapCoords(mapX, mapY))) {
                return true;
            }
        }
    }
    return false;
}

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
