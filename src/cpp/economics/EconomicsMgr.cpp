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
                int mapX, mapY;
                building->getMapCoords(mapX, mapY);

                Carrier* carrier = new Carrier(result.building, result.route, result.goods);
                carrier->setMapCoords(mapX, mapY);

                building->carrier = carrier;
            }
        }

        // Träger unterwegs? fortbewegen
        else if (building->carrier != nullptr) {
            Carrier* carrier = building->carrier;
            Animation* animation = graphicsMgr->getAnimation(CARRIER); // TODO später muss die verwendete Animation am Carrier hängen

            // Bewegen
            carrier->animationFrame += (double) ticksPastSinceLastUpdate / 1000 * animation->getFps();
            while (carrier->animationFrame >= animation->getFramesCount()) {
                carrier->animationFrame -= animation->getFramesCount();
            }
            // TODO Position entsprechend der Route ändern
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