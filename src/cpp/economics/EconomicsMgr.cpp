#include "config/BuildingConfigMgr.h"
#include "economics/EconomicsMgr.h"
#include "game/Colony.h"
#include "game/Game.h"

// Aus main.cpp importiert
extern BuildingConfigMgr* buildingConfigMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;


EconomicsMgr::EconomicsMgr() {
}

EconomicsMgr::~EconomicsMgr() {
}

void EconomicsMgr::update(Structure* structure) {
    // Erstmal nur Gebäude machen
    Building* building = dynamic_cast<Building*>(structure);
    if (building == nullptr) {
        return;
    }

    StructureType structureType = building->getStructureType();
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(structureType);

    // Produktion durchführen
    Uint32 ticksNow = SDL_GetTicks();
    Uint32 ticksPastSinceLastUpdate = ticksNow - building->getLastUpdateTime();

    // TODO Produktion abhängig machen von Input-Waren und deren Verfügbarkeit
    double goodsProduced = (double)ticksPastSinceLastUpdate / 60000 * buildingConfig->productionRate;
    building->productionSlots.output.increaseInventory(goodsProduced);

    // Gebäude, die Waren einsammeln:

    // TODO erstmal nur fix für Webstuben
    if (structureType == StructureType::WEAVING_MILL1) {
        // Ist der Träger zu Hause? Gucken, wo was zu holen is und Abholung einleiten
        if (building->carrier == nullptr) {

            FindBuildingToGetGoodsFromResult result = findBuildingToGetGoodsFrom(building);
            if (result.building != nullptr) {
                // Träger anlegen und zuweisen
                MapCoordinate firstHopOnRoute = result.route->front();

                Carrier* carrier = new Carrier(result.building, result.route, result.goods, true);
                carrier->setMapCoords(firstHopOnRoute.mapX, firstHopOnRoute.mapY);

                building->carrier = carrier;
            }
        }

        // Träger unterwegs? fortbewegen
        else if (building->carrier != nullptr) {
            Carrier* carrier = building->carrier;
            Animation* animation = graphicsMgr->getAnimation(CARRIER); // TODO später muss die verwendete Animation am Carrier hängen

            // Animieren
            carrier->animationFrame += (double) ticksPastSinceLastUpdate / 1000 * animation->getFps();
            while (carrier->animationFrame >= animation->getFramesCount()) {
                carrier->animationFrame -= animation->getFramesCount();
            }

            // Bewegen
            double speed = 0.75; // erstmal fix, die Animation muss man eh nochmal anpassen, weil das Männchen mehr schwebt, als läuft

            MapCoordinate nextHopOnRoute = *carrier->nextHopInRoute;
            double stepX = speed * (nextHopOnRoute.mapX - carrier->mapX);
            double stepY = speed * (nextHopOnRoute.mapY - carrier->mapY);

            // TODO Wir machen einen minimalen Fehler, dass wir bei Erreichen der nächsten Kachel, wieder auf 0 setzen
            // und nicht versuchen, den "Rest" der vergangenen Zeit anzuwenden und ein Stückchen weiterzulaufen.

            carrier->mapXFraction += (double) ticksPastSinceLastUpdate / 1000 * stepX;
            carrier->mapYFraction += (double) ticksPastSinceLastUpdate / 1000 * stepY;
            bool hopReached = false;

            if (carrier->mapXFraction <= -1) {
                carrier->mapXFraction = 0;
                carrier->mapX--;
                hopReached = true;
            } else if (carrier->mapXFraction >= 1) {
                carrier->mapXFraction = 0;
                carrier->mapX++;
                hopReached = true;
            }
            if (carrier->mapYFraction <= -1) {
                carrier->mapYFraction = 0;
                carrier->mapY--;
                hopReached = true;
            } else if (carrier->mapYFraction >= 1) {
                carrier->mapYFraction = 0;
                carrier->mapY++;
                hopReached = true;
            }

            // Wir haben den nächsten Schritt in der Route erreicht
            if (hopReached) {
                carrier->nextHopInRoute++;

                // Route fertig?
                if (carrier->nextHopInRoute == carrier->route->cend()) {
                    MapTile* mapTile = game->getMap()->getMapTileAt(carrier->mapX, carrier->mapY);
                    Building* targetBuilding = dynamic_cast<Building*>(mapTile->mapObject);

                    // targetBuilding == nullptr -> Nutzer hat die Map inzwischen geändert und das Zielgebäude abgerissen
                    // Der Träger verschwindet einfach
                    if (targetBuilding == nullptr) {
                        delete carrier;
                        building->carrier = nullptr;
                    }

                    // Das war Hinweg -> Waren aufladen und Träger auf Rückweg schicken
                    else if (carrier->onOutboundTrip) {
                        // Waren aufladen
                        int goodsWeCollect = (int) targetBuilding->productionSlots.output.inventory;
                        targetBuilding->productionSlots.output.inventory -= goodsWeCollect;

                        Route* route = carrier->route;
                        Route* returnRoute = AStar::findRoute(route->back(), route->front(), building);

                        delete carrier;
                        building->carrier = nullptr;

                        // Es kann sein, dass es keine Rückroute gibt, wenn der Nutzer inzwischen die Map verändert hat.
                        // In diesem Fall hat er Pech gehabt, die Waren verschwinden mit dem Träger
                        if (returnRoute != nullptr) {
                            AStar::cutRouteInsideBuildings(returnRoute);

                            MapCoordinate firstHopOnReturnRoute = returnRoute->front();

                            Carrier* returnCarrier = new Carrier(
                                building, returnRoute, targetBuilding->productionSlots.output.goodsType, false);
                            returnCarrier->setMapCoords(firstHopOnReturnRoute.mapX, firstHopOnReturnRoute.mapY);
                            returnCarrier->carriedGoods.inventory = goodsWeCollect;

                            building->carrier = returnCarrier;
                        }
                    }

                    // Das war Rückweg -> Waren ausladen und Träger zerstören
                    else if (!carrier->onOutboundTrip) {
                        // Gucken, in welchen Slot die Waren müssen
                        if (building->productionSlots.input.goodsType == carrier->carriedGoods.goodsType) {
                            building->productionSlots.input.increaseInventory(carrier->carriedGoods.inventory);
                        }
                        else if (building->productionSlots.input2.goodsType == carrier->carriedGoods.goodsType) {
                            building->productionSlots.input2.increaseInventory(carrier->carriedGoods.inventory);
                        }

                        delete carrier;
                        building->carrier = nullptr;
                    }
                }
            }
        }
    }

    building->setLastUpdateTime(ticksNow);
}

FindBuildingToGetGoodsFromResult EconomicsMgr::findBuildingToGetGoodsFrom(Building* building) {
    StructureType structureType = building->getStructureType();
    const BuildingConfig* buildingConfig = buildingConfigMgr->getConfig(structureType);
    const RectangleData<char>* catchmentArea = buildingConfig->getCatchmentArea();
    if (catchmentArea == nullptr) {
        return FindBuildingToGetGoodsFromResult(); // kein Einzugsbereich
    }

    // Welche Waren brauchen wir überhaupt?
    GoodsType goodsRequired1 = buildingConfig->getBuildingProduction()->input.goodsType;
    GoodsType goodsRequired2 = buildingConfig->getBuildingProduction()->input2.goodsType;
    if (goodsRequired1 == GoodsType::NO_GOODS && goodsRequired2 == GoodsType::NO_GOODS) {
        return FindBuildingToGetGoodsFromResult(); // Wir brauchen keine Waren
    }

    // Meinen Einzugsbereich durchsehen (TODO: und Liste aller Gebäude innerhalb anfertigen)
    Map* map = game->getMap();
    for (int y = 0; y < catchmentArea->height; y++) {
        for (int x = 0; x < catchmentArea->width; x++) {
            int mapX, mapY, mapWidth, mapHeight;
            building->getMapCoords(mapX, mapY, mapWidth, mapHeight);

            mapX += x - (catchmentArea->width - mapWidth) / 2;
            mapY += y - (catchmentArea->height - mapHeight) / 2;

            // Gebäude da?
            MapTile* mapTile = map->getMapTileAt(mapX, mapY);
            if (mapTile == nullptr) {
                continue;
            }

            Building* buildingThere = dynamic_cast<Building*>(mapTile->mapObject);
            if (buildingThere == nullptr) {
                continue;
            }

            // Liefert das Gebäude was passendes?
            const BuildingConfig* buildingThereConfig = buildingConfigMgr->getConfig(buildingThere->getStructureType());
            if (buildingThereConfig->getBuildingProduction()->output.goodsType != goodsRequired1 &&
                buildingThereConfig->getBuildingProduction()->output.goodsType != goodsRequired2) {

                continue; // produziert nix passendes
            }

            // Waren zum Abholen da?
            if (buildingThere->productionSlots.output.inventory < 1) {
                continue;
            }

            // Checken, ob eine Route dahin existiert
            int mapXSource, mapYSource;
            building->getMapCoords(mapXSource, mapYSource);
            MapCoordinate mapCoordinateSource = MapCoordinate(mapXSource, mapYSource);

            int mapXDestination, mapYDestination;
            buildingThere->getMapCoords(mapXDestination, mapYDestination);
            MapCoordinate mapCoordinateDestination = MapCoordinate(mapXDestination, mapYDestination);

            Route* route = AStar::findRoute(mapCoordinateSource, mapCoordinateDestination, building);
            if (route == nullptr) {
                continue; // gibt keinen Weg dahin
            }

            AStar::cutRouteInsideBuildings(route);

            // Juhuu! Dieses Gebäude kommt in Frage
            FindBuildingToGetGoodsFromResult result;
            result.building = buildingThere;
            result.route = route;
            result.goods = buildingThereConfig->getBuildingProduction()->output.goodsType;
            return result; // TODO Testweise nehmen wir das erstbeste Gebäude
        }
    }

    // TODO Liste sortieren und besten Treffer wählen
    // TODO Es muss verhindert werden, dass zwei Träger zum selben Ziel unterwegs sind

    return FindBuildingToGetGoodsFromResult();
}