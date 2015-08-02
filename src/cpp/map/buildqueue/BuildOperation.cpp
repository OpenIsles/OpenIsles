#include <random>
#include "game/Game.h"
#include "map/buildqueue/BuildOperation.h"


void BuildOperation::requestBuild(const MapCoords& mapCoords, const MapObjectType* mapObjectType,
                                  const FourthDirection& view) {

    // TODO BUILDOPERATION die untenstehende Prüfung entsprechend einem Teil von isAllowedToPlaceMapObject()

    // Erst überprüfen, ob sich das neue Objekt mit einem bestehenden in der Queue überlappt
    unsigned char mapWidth, mapHeight;
    if (view == Direction::NORTH || view == Direction::SOUTH) {
        mapWidth = mapObjectType->mapWidth;
        mapHeight = mapObjectType->mapHeight;
    } else {
        mapWidth = mapObjectType->mapHeight;
        mapHeight = mapObjectType->mapWidth;
    }

    for (int y = 0, my = mapCoords.y(); y < mapHeight; y++, my++) {
        for (int x = 0, mx = mapCoords.x(); x < mapWidth; x++, mx++) {
            auto iter = result.find(MapCoords(mx, my));
            if (iter != result.cend()) {
                if (!iter->second->costsNothingBecauseOfChange) {
                    /*
                     * Überlappung gefunden? Dann abbrechen, wir ignorieren den Aufruf.
                     * Allerdings nur, wenn das ResultBit costsNothingBecauseOfChange==false hat,
                     * da wir nur abbrechen, wenn ein bewusster Bau vorliegt.
                     * Wenn ein Bauauftrag für Kachel A eine Ersetzung für Kachel B zur Folge hat,
                     * dann darf natürlich nachträglich ein Bauauftrag für Kachel B vergeben werden.
                     */
                    return;
                }
            }
        }
    }

    // Kolonie prüfen: Alle Objekte in der Bau-Queue müssen in derselben Kolonie sein!
    if (colony == nullptr) {
        // Erste Map-Objekt gibt die Kolonie vor
        colony = context->game->getColony(mapCoords);
        if (colony == nullptr) {
            return; // Keine Kolonie, da darf man eh nicht bauen
        }

    } else {
        // Jedes weitere Map-Objekt muss in dieser Kolonie sein
        Colony* colonyThere = context->game->getColony(mapCoords);
        if ((colony == nullptr) || (colonyThere != colony)) {
            return;
        }
    }

    // Sonderfälle, wo zufällig gewählt wird: Haus (= zufälliges Pionier-Haus) und Wald
    // TODO über Config steuern
    if (mapObjectType->name == "pioneers-house1" || mapObjectType->name == "northern-forest1") {
        std::random_device randomDevice;
        std::default_random_engine randomEngine(randomDevice());

        if (mapObjectType->name == "pioneers-house1") {
            std::uniform_int_distribution<int> randomPioneerHouse(1, 5);
            std::string mapObjectTypeName = "pioneers-house" + toString(randomPioneerHouse(randomEngine));

            mapObjectType = context->configMgr->getMapObjectType(mapObjectTypeName);
        } else if (mapObjectType->name == "northern-forest1") {
            std::uniform_int_distribution<int> randomNorthernForest(1, 2);
            std::string mapObjectTypeName = "northern-forest" + toString(randomNorthernForest(randomEngine));

            mapObjectType = context->configMgr->getMapObjectType(mapObjectTypeName);
        }
    }

    // Objekt in die Bauqueue aufnehmen und Result-Objekt aktualisieren
    mapObjectsToBuild.push_back({ mapCoords, mapObjectType, view });
    rebuildResult();
}

void BuildOperation::requestBuildWhenNothingInTheWay(const MapCoords& mapCoords, const MapObjectType* mapObjectType,
                                                     const FourthDirection& view) {

    // Überprüfen. ob auf der Karte alles frei is,...
    if (isSomethingInTheWayOnTheMap({ mapCoords, mapObjectType, view })) {
        return;
    }

    // ...dann requestBuild() übergeben
    requestBuild(mapCoords, mapObjectType, view);
}

bool BuildOperation::isSomethingInTheWayOnTheMap(const MapObjectToBuild& mapObjectToBuild) const {
    const MapObjectType* mapObjectType = mapObjectToBuild.mapObjectType;

    unsigned char mapWidth, mapHeight;
    if (mapObjectToBuild.view == Direction::NORTH || mapObjectToBuild.view == Direction::SOUTH) {
        mapWidth = mapObjectType->mapWidth;
        mapHeight = mapObjectType->mapHeight;
    } else {
        mapWidth = mapObjectType->mapHeight;
        mapHeight = mapObjectType->mapWidth;
    }

    // Gucken, ob alles frei is
    for (int y = 0, my = mapObjectToBuild.mapCoords.y(); y < mapHeight; y++, my++) {
        for (int x = 0, mx = mapObjectToBuild.mapCoords.x(); x < mapWidth; x++, mx++) {
            MapCoords mapCoords(mx, my);

            // Überlappung in der Bauqueue! Hier darf auf keinen Fall gebaut werden
            if (result.find(mapCoords) != result.cend()) {
                return false;
            }

            MapObjectFixed* mapObjectFixedAlreadyThere = context->game->getMap()->getMapObjectFixedAt(mapCoords);
            if (mapObjectFixedAlreadyThere != nullptr) {
                // Überbauen erlaubt?
                if (!(mayBuildOver(mapObjectFixedAlreadyThere->getMapObjectType(), mapObjectType))) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool BuildOperation::mayBuildOver(const MapObjectType* mapObjectTypeThere,
                                  const MapObjectType* mapObjectTypeToBuildOver) const {

    // TODO allgemeiner hinkriegen; ggf. notwendig, dass nur ein MapObjectType "Feldweg"/"Pflasterstraße" existiert
    // TODO via Config steuerbar

    // Ersetzung für Straßen erlauben
    bool isStreetThere = (mapObjectTypeThere->type == MapObjectTypeClass::STREET);
    bool isStreetToBuildOver = (mapObjectTypeToBuildOver->type == MapObjectTypeClass::STREET);

    if (isStreetThere && isStreetToBuildOver) {
        return true;
    }

    // TODO weitere Konstellationen (Plätze über Weg/Straße, Gebäude über Harvestables)

    return false;
}

StreetConnections BuildOperation::calculateStreetConnections(const MapCoords& mapCoords) const {
    // Angrenzende Straßen (auf der Karte und im BuildOperationResult) checken
    const Map* map = context->game->getMap();
    auto isStreetThere = [&](const MapCoords& mapCoordsInLamdba) {
        if (map->isStreetAt(mapCoordsInLamdba)) {
            return true;
        }

        auto iter = result.find(mapCoordsInLamdba);
        if (iter != result.cend()) {
            return (std::dynamic_pointer_cast<Street>(iter->second->mapObjectToReplaceWith) != nullptr);
        }

        return false;
    };

    bool isStreetNorth = isStreetThere(MapCoords(mapCoords.x(), mapCoords.y() - 1));
    bool isStreetEast = isStreetThere(MapCoords(mapCoords.x() + 1, mapCoords.y()));
    bool isStreetSouth = isStreetThere(MapCoords(mapCoords.x(), mapCoords.y() + 1));
    bool isStreetWest = isStreetThere(MapCoords(mapCoords.x() - 1, mapCoords.y()));

    StreetConnections streetConnections;
    streetConnections.set(StreetConnections::BIT_POS_NORTH, isStreetNorth);
    streetConnections.set(StreetConnections::BIT_POS_EAST, isStreetEast);
    streetConnections.set(StreetConnections::BIT_POS_SOUTH, isStreetSouth);
    streetConnections.set(StreetConnections::BIT_POS_WEST, isStreetWest);

    return streetConnections;
}

void BuildOperation::adeptExistingStreets(const Street& streetToAdeptAround) {
    Map* map = context->game->getMap();

    auto adeptExistingStreet = [&](const MapCoords& mapCoords) {
        // Ist hier eine bestehende Straße?
        const Street* existingStreet = dynamic_cast<const Street*>(map->getMapObjectFixedAt(mapCoords));
        if (existingStreet == nullptr) {
            return;
        }

        // Nicht ersetzen, wenn dort im BuildOperationResult bereits was neue gebaut wird
        auto iter = result.find(mapCoords);
        if (iter != result.cend()) {
            return;
        }

        // Prüfen, ob Ersetzung notwendig ist
        StreetConnections streetConnectionsShouldBe = calculateStreetConnections(existingStreet->getMapCoords());
        if (streetConnectionsShouldBe == existingStreet->streetConnections &&
            existingStreet->getView() == Direction::SOUTH) {

            return; // passt alles schon
        }

        // Ersetzung anlegen
        assert((existingStreet->getMapWidth() == 1) && (existingStreet->getMapHeight() == 1));
        assert((existingStreet->getMapWidth() == 1) && (existingStreet->getMapHeight() == 1));

        Street* mapObjectToReplaceWith = (Street*) MapObjectFixed::instantiate(existingStreet->getMapObjectType());
        mapObjectToReplaceWith->setMapCoords(mapCoords);
        mapObjectToReplaceWith->setView(Direction::SOUTH);
        mapObjectToReplaceWith->setMapWidth(existingStreet->getMapWidth());
        mapObjectToReplaceWith->setMapHeight(existingStreet->getMapHeight());
        mapObjectToReplaceWith->streetConnections = streetConnectionsShouldBe;

        std::shared_ptr<BuildOperationResultBit> resultBit(new BuildOperationResultBit());
        resultBit->somethingInTheWay = false;
        resultBit->costsNothingBecauseOfChange = true;
        resultBit->resourcesEnoughToBuildThis = true;
        resultBit->mapObjectToReplaceWith.reset(mapObjectToReplaceWith);
        resultBit->mapObjectToDraw = resultBit->mapObjectToReplaceWith;

        result[mapCoords] = resultBit;
    };

    const MapCoords& mapCoords = streetToAdeptAround.getMapCoords();
    adeptExistingStreet(MapCoords(mapCoords.x(), mapCoords.y() - 1));
    adeptExistingStreet(MapCoords(mapCoords.x() + 1, mapCoords.y()));
    adeptExistingStreet(MapCoords(mapCoords.x(), mapCoords.y() + 1));
    adeptExistingStreet(MapCoords(mapCoords.x() - 1, mapCoords.y()));
}

void BuildOperation::rebuildResult() {
    result.clear();
    result.result = BuildOperationResult::OK;

    if (mapObjectsToBuild.empty()) {
        return;
    }

    BuildingCosts resourcesAvailable = {
        player.coins,
        (int) colony->getGoods(context->configMgr->getGood("tools")).inventory,
        (int) colony->getGoods(context->configMgr->getGood("wood")).inventory,
        (int) colony->getGoods(context->configMgr->getGood("bricks")).inventory
    };

    bool enoughResources = true;
    for (const MapObjectToBuild& mapObjectToBuild : mapObjectsToBuild) {
        const MapObjectType* mapObjectType = mapObjectToBuild.mapObjectType;

        unsigned char mapWidth, mapHeight;
        if (mapObjectToBuild.view == Direction::NORTH || mapObjectToBuild.view == Direction::SOUTH) {
            mapWidth = mapObjectType->mapWidth;
            mapHeight = mapObjectType->mapHeight;
        } else {
            mapWidth = mapObjectType->mapHeight;
            mapHeight = mapObjectType->mapWidth;
        }

        std::shared_ptr<BuildOperationResultBit> resultBit(new BuildOperationResultBit());

        // Was im Weg?
        bool somethingInTheWay = isSomethingInTheWayOnTheMap(mapObjectToBuild);
        resultBit->somethingInTheWay = somethingInTheWay;

        if (!somethingInTheWay) {
            MapObjectFixed* mapObjectToReplaceWith = MapObjectFixed::instantiate(mapObjectType);
            mapObjectToReplaceWith->setMapCoords(mapObjectToBuild.mapCoords);
            mapObjectToReplaceWith->setView(mapObjectToBuild.view);
            mapObjectToReplaceWith->setMapWidth(mapWidth);
            mapObjectToReplaceWith->setMapHeight(mapHeight);
            resultBit->mapObjectToReplaceWith.reset(mapObjectToReplaceWith);

            // TODO BUILDOPERATION Spezialfall Straßen: mapObjectToDraw = Kreuzung, aber wegen Resourcenmangel mapObjectToReplaceWith = T-Stück

            resultBit->mapObjectToDraw = resultBit->mapObjectToReplaceWith;
        }
        else {
            result.result = BuildOperationResult::SOMETHING_IN_THE_WAY;
        }

        bool costsNothingBecauseOfChange = false;
        // Gratis-Überbauen geht sowieso nur mit Straßen, die immer 1x1 groß sind
        if (mapObjectType->type == MapObjectTypeClass::STREET) {
            assert(mapWidth == 1 && mapHeight == 1);

            // Wurde mit demselben Map-Objekt-Typ überbaut, so ist das nur eine "Änderung" und kostet nix
            MapObjectFixed* mapObjectFixedAlreadyThere =
                context->game->getMap()->getMapObjectFixedAt(mapObjectToBuild.mapCoords);
            if (mapObjectFixedAlreadyThere != nullptr) {
                if (mapObjectFixedAlreadyThere->getMapObjectType() == mapObjectType) {
                    costsNothingBecauseOfChange = true;
                }
            }
        }
        resultBit->costsNothingBecauseOfChange = costsNothingBecauseOfChange;

        // Resourcen überprüfen (einmal false geworden, sparen wir uns die Differenz-Bildung)
        if (!costsNothingBecauseOfChange) {
            if (enoughResources) {
                resourcesAvailable -= mapObjectType->buildingCosts;
                enoughResources = resourcesAvailable.isNonNegative();
            }
            resultBit->resourcesEnoughToBuildThis = enoughResources;
        } else {
            // Wenns gratis is, reichen die Resourcen freilich :-)
            resultBit->resourcesEnoughToBuildThis = true;
        }

        // ResultBit den Koordinaten zuweisen
        for (int y = 0, my = mapObjectToBuild.mapCoords.y(); y < mapHeight; y++, my++) {
            for (int x = 0, mx = mapObjectToBuild.mapCoords.x(); x < mapWidth; x++, mx++) {
                result[MapCoords(mx, my)] = resultBit;
            }
        }
    }

    // Jetzt für Straßen streetConnections setzen, nachdem alles fertig is.
    // Wichtig: Result abkopieren, der Iterator funktioniert nicht korrekt, wenn sich in der Schleife der
    // Containerinhalt, den er iteriert, ändert
    BuildOperationResult resultCopy = result;

    for (auto iter : resultCopy) {
        std::shared_ptr<BuildOperationResultBit>& resultBit = iter.second;

        if (resultBit->mapObjectToReplaceWith) {
            std::shared_ptr<Street> streetPtr = std::dynamic_pointer_cast<Street>(resultBit->mapObjectToReplaceWith);
            if (streetPtr) {
                Street& street = *streetPtr;

                // streetConnections setzen
                StreetConnections streetConnections = calculateStreetConnections(street.getMapCoords());
                street.streetConnections = streetConnections;

                // mindestens eine angrenzende Straße? Dann die View auf SOUTH stellen.
                if (streetConnections.any()) {
                    street.setView(Direction::SOUTH);
                }

                // Straßen außenrum ggf. ersetzen
                // (für Ersetzungen können wir sparen, diese triggern niemals eine zweite Ersetzung)
                if (!resultBit->costsNothingBecauseOfChange) {
                    adeptExistingStreets(street);
                }
            }
        }
    }
}

void BuildOperation::doBuild() {
    // TODO BUILDOPERATION

    //    Player* currentPlayer = context->game->getCurrentPlayer();
//
//    // Der Reihe nach alle zu setzenden Gebäude durchgehen. Gehen die Resourcen aus, brechen wir ab.
//    for (auto iter = buildQueue.cbegin(); iter != buildQueue.cend(); iter++) {
//        const MapObjectFixed& mapObject = **iter;
//
//        const MapObjectType* mapObjectType = mapObject.getMapObjectType();
//        const MapCoords& mapCoords = mapObject.getMapCoords();
//
//        const FourthDirection& view = mapObject.getView();
//        unsigned char isAllowedToPlaceResult = isAllowedToPlaceMapObject(mapCoords, mapObjectType, view);
//        if ((isAllowedToPlaceResult & ~PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY) != PLACING_STRUCTURE_ALLOWED) {
//            // Dürfen wir nicht mehr setzen (zwischenzeitlich was im Weg oder Resourcen ausgegangen), dann abbrechen
//            break;
//        }
//
//        // Ok, Gebäude nun wirklich setzen
//        context->game->addMapObjectFixed(mapCoords, mapObjectType, view, currentPlayer);
//
//        // Resourcen bezahlen
//        Colony* colony = context->game->getColony(
//            currentPlayer, context->game->getMap()->getMapTileAt(mapCoords)->isle);
//        const BuildingCosts& buildingCosts = mapObjectType->buildingCosts;
//
//        currentPlayer->coins -= buildingCosts.coins;
//        colony->getGoods(toolsGood).inventory -= buildingCosts.tools;
//        colony->getGoods(woodGood).inventory -= buildingCosts.wood;
//        colony->getGoods(bricksGood).inventory -= buildingCosts.bricks;
//    }
//    buildQueue.clear();
//

}




//
//unsigned char GuiMap::isAllowedToPlaceMapObject(
//    const MapCoords& mapCoords, const MapObjectType* mapObjectType, const FourthDirection& view) const {
//
//    MapTile* mapTile = context->game->getMap()->getMapTileAt(mapCoords);
//    if (mapTile == nullptr) {
//        return PLACING_STRUCTURE_OUTSIDE_OF_ISLE;
//    }
//
//    Isle* isle = mapTile->isle;
//    if (isle == nullptr) {
//        return PLACING_STRUCTURE_OUTSIDE_OF_ISLE;
//    }
//
//    unsigned char result = PLACING_STRUCTURE_ALLOWED;
//
//    // Resourcen checken
//    Player* currentPlayer = context->game->getCurrentPlayer();
//    Colony* colony = context->game->getColony(currentPlayer, isle);
//
//    if (colony != nullptr) {
//        const BuildingCosts& buildingCosts = mapObjectType->buildingCosts;
//        if ((buildingCosts.coins > currentPlayer->coins) ||
//            (buildingCosts.tools > colony->getGoods(toolsGood).inventory) ||
//            (buildingCosts.wood > colony->getGoods(woodGood).inventory) ||
//            (buildingCosts.bricks > colony->getGoods(bricksGood).inventory)) {
//            result |= PLACING_STRUCTURE_NO_RESOURCES;
//        }
//    }
//
//    // Checken, ob alles frei und erlaubt is, um das Gebäude zu setzen
//    unsigned char mapWidth, mapHeight;
//    if (view == Direction::NORTH || view == Direction::SOUTH) {
//        mapWidth = mapObjectType->mapWidth;
//        mapHeight = mapObjectType->mapHeight;
//    } else {
//        mapWidth = mapObjectType->mapHeight;
//        mapHeight = mapObjectType->mapWidth;
//    }
//
//    for (int y = mapCoords.y(); y < mapCoords.y() + mapHeight; y++) {
//        for (int x = mapCoords.x(); x < mapCoords.x() + mapWidth; x++) {
//            const MapTile* mapTile = context->game->getMap()->getMapTileAt(MapCoords(x, y));
//
//            // Steht was im Weg?
//            if (mapTile->mapObjectFixed != nullptr) {
//                result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY;
//            }
//
//            auto iterMapTileTemporarily = mapTilesToDrawTemporarily.find(MapCoords(x, y));
//            if (iterMapTileTemporarily != mapTilesToDrawTemporarily.cend()) {
//                if (iterMapTileTemporarily->second.mapObjectToDraw) {
//                    // Zwar noch nicht echt gebaut, allerdings wollen wir da was hinbauen.
//                    result |= PLACING_STRUCTURE_SOMETHING_IN_THE_WAY_TEMPORARILY;
//                }
//            }
//
//            // Passt das Gelände?
//            const MapTileConfig* mapTileConfig = mapTile->getMapTileConfig();
//            if (((MapTileTypeInt) mapTileConfig->mapTileType & mapObjectType->placeableOnMapTileTypeMask) == 0) {
//                result |= PLACING_STRUCTURE_MAP_TILE_TYPE_MISMATCH;
//            }
//
//            // Gebiet erschlossen?
//            if (mapTile->player == nullptr ||                             // Gebiet nicht erschlossen,
//                mapTile->player != context->game->getCurrentPlayer()) {   // oder nicht unseres
//
//                result |= PLACING_STRUCTURE_ROOM_NOT_UNLOCK;
//            }
//        }
//    }
//
//    return result;
//}
