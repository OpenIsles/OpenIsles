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
            if (result.find(MapCoords(mx, my)) != result.cend()) {
                return; // Überlappung gefunden? Dann abbrechen, wir ignorieren den Aufruf.
            }
        }
    }

    // Objekt in die Bauqueue aufnehmen und Result-Objekt aktualisieren
    mapObjectsToBuild.push_front({ mapCoords, mapObjectType, view });
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


bool BuildOperation::isSomethingInTheWayOnTheMap(const MapObjectToBuild& mapObjectToBuild) {
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

            MapObjectFixed* mapObjectFixedAlreadyThere = game.getMap()->getMapObjectFixedAt(mapCoords);
            if (mapObjectFixedAlreadyThere != nullptr) {
                // TODO BUILDOPERATION Überbauen ermöglichen
                return true;
            }
        }
    }

    return false;
}


// TODO BUILDOPERATION noch ganz viel zu tun....
void BuildOperation::rebuildResult() {
    assert(!mapObjectsToBuild.empty());

    result.clear();
    result.result = BuildOperationResult::OK;

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

        bool somethingInTheWay = isSomethingInTheWayOnTheMap(mapObjectToBuild);

        // ResultBit anlegen
        std::shared_ptr<BuildOperationResultBit> resultBit(new BuildOperationResultBit());

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

        resultBit->resourcesEnoughToBuildThis = true; // TODO BUILDOPERATION

        // ResultBit den Koordinaten zuweisen
        for (int y = 0, my = mapObjectToBuild.mapCoords.y(); y < mapHeight; y++, my++) {
            for (int x = 0, mx = mapObjectToBuild.mapCoords.x(); x < mapWidth; x++, mx++) {
                result[MapCoords(mx, my)] = resultBit;
            }
        }
    }

    // Erstes Map-Objekt (wenn ein Gebäude) kriegt den Einzugsbereich gemalt
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
