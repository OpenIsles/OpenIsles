#include <array>
#include <string>
#include "game/Colony.h"
#include "game/Game.h"
#include "game/GameIO.h"
#include "map/coords/ScreenCoords.h"
#include "map/Map.h"
#include "utils/Consts.h"


// TODO allgemein: Fehlermanagement, wenn die Datei mal nicht so hübsch aussieht, dass alle Tags da sind

void GameIO::loadGameFromTMX(
    Game* game, const ConfigMgr* configMgr, const IGraphicsMgr* graphicsMgr, const char* filename) {

    // TODO aktuell kann diese Methode nur mit einem frischen Game-Objekt aufgerufen werden. Nochmal laden = vorher saubermachen

    // Datei öffnen
    rapidxml::file<> tmxFile(filename);

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(tmxFile.data());

    // Kartengröße einlesen und Map anlegen
    rapidxml::xml_node<>* mapNode = xmlDocument->first_node("map", 3, true);
    int newMapWidth = atoi(mapNode->first_attribute("width", 5, true)->value());
    int newMapHeight = atoi(mapNode->first_attribute("height", 6, true)->value());
    game->getMap()->initNewMap(newMapWidth, newMapHeight);

    // Ebenen durchgehen.
    rapidxml::xml_node<>* objectgroupIslesNode = nullptr;
    rapidxml::xml_node<>* objectgroupColoniesNode = nullptr;
    rapidxml::xml_node<>* objectgroupStructuresNode = nullptr;

    for (rapidxml::xml_node<>* objectgroupNode = mapNode->first_node("objectgroup", 11, true); objectgroupNode != nullptr;
         objectgroupNode = objectgroupNode->next_sibling("objectgroup", 11, true)) {

        const char* layerName = objectgroupNode->first_attribute("name", 4, true)->value();

        if (strcmp(layerName, "isles") == 0) {
            objectgroupIslesNode = objectgroupNode;
        } else if (strcmp(layerName, "colonies") == 0) {
            objectgroupColoniesNode = objectgroupNode;
        } else if (strcmp(layerName, "structures") == 0) {
            objectgroupStructuresNode = objectgroupNode;
        }
    }

    // Spieler laden
    loadPlayers(game, mapNode);

    // Karte und Inseln laden
    loadMap(game, configMgr, graphicsMgr, objectgroupIslesNode);

    // Siedlungen laden
    loadColonies(game, objectgroupColoniesNode);

    // Strukturen und Gebäude laden
    loadStructures(game, objectgroupStructuresNode);

    // XML-Document wegräumen
    delete xmlDocument;
}

void GameIO::loadPlayers(Game* game, rapidxml::xml_node<>* mapNode) {
    rapidxml::xml_node<>* propertiesNode = mapNode->first_node("properties", 10, true);

    int currentPlayerNr = atoi(getPropertyValueFromPropertiesNode(propertiesNode, "current_player"));

    for (int i = 1; i <= 4; i++) {
        std::string playerAttrPrefix = "player" + toString(i) + "_";

        // Spielerfarbe
        std::string playerColorAttrName = playerAttrPrefix + "color";
        const char* playerColorAttrValue = getPropertyValueFromPropertiesNode(propertiesNode, playerColorAttrName.c_str());
        PlayerColor playerColor;

        if (strcmp(playerColorAttrValue, "red") == 0) {
            playerColor = PlayerColor::RED;
        } else if (strcmp(playerColorAttrValue, "blue") == 0) {
            playerColor = PlayerColor::BLUE;
        } else if (strcmp(playerColorAttrValue, "yellow") == 0) {
            playerColor = PlayerColor::YELLOW;
        } else if (strcmp(playerColorAttrValue, "white") == 0) {
            playerColor = PlayerColor::WHITE;
        } else {
            throw new std::runtime_error("Illegal player color");
        }

        // Spielername
        std::string playerNameAttrName = playerAttrPrefix + "name";
        std::string playerName = getPropertyValueFromPropertiesNode(propertiesNode, playerNameAttrName.c_str());

        // Spieler anlegen
        Player* player = new Player(playerColor, playerName);
        game->addPlayer(player);

        if (i == currentPlayerNr) {
            game->setCurrentPlayer(player);
        }
    }
}

void GameIO::loadMap(
    Game* game, const ConfigMgr* configMgr, const IGraphicsMgr* graphicsMgr, rapidxml::xml_node<>* objectgroupIslesNode) {

    Map* map = game->getMap();

    // Inseln einlesen
    for (rapidxml::xml_node<>* objectNode = objectgroupIslesNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);

        // Insel
        if (strcmp(nodeType, "isle") == 0) {
            // Objekt aus der Tiled-Datei lesen
            const char* isleName = objectNode->first_attribute("name", 4, true)->value();

            int isleWidth = atoi(objectNode->first_attribute("width", 5, true)->value());
            int isleHeight = atoi(objectNode->first_attribute("height", 6, true)->value());

            int isleMapWidth = isleWidth / IGraphicsMgr::TILE_HEIGHT; // tiled rechnet merkwürdigerweise auch für X in KachelHÖHE
            int isleMapHeight = isleHeight / IGraphicsMgr::TILE_HEIGHT;

            // Dateiname der Insel zusammenbauen
            std::string isleFilename = "data/map/isles/";
            isleFilename.append(isleName);
            isleFilename.append(".tmx");

            // Insel laden
            Isle* isle = new Isle(isleFilename.c_str());

            // Prüfen, ob die Insel wirklich die Größe hat, wie die Karte sie haben will.
            if (isle->getWidth() != isleMapWidth || isle->getHeight() != isleMapHeight) {
                std::cerr << "Isle '" << isleName << "' ('" <<
                    toString(isle->getWidth()) << "x" << toString(isle->getHeight()) <<
                    ") does not match size on map (" <<
                    toString(isleMapWidth) << "x" << toString(isleMapHeight) << ")";

                throw new std::runtime_error("Isle does not match size on map");
            }

            // Insel zur Karte hinzufügen
            isle->setMapCoords(mapCoords);
            isle->setMapWidth(isleMapWidth);
            isle->setMapHeight(isleMapHeight);
            map->isles.push_back(isle);

            for (int my = mapCoords.y(), isleY = 0; my < mapCoords.y() + isleMapHeight; my++, isleY++) {
                for (int mx = mapCoords.x(), isleX = 0; mx < mapCoords.x() + isleMapWidth; mx++, isleX++) {
                    MapTile* mapTile = map->mapTiles->getData(mx, my, nullptr);
                    if (mapTile == nullptr) {
                        continue; // TODO Warnung ausgeben, dass die Map nicht ok is. Wir haben eine Insel, die außerhalb der Karte liegt
                    }

                    unsigned char tileIndex = isle->getTileAt(isleX, isleY);
                    const MapTileConfig* mapTileConfig = configMgr->getMapTileConfigByTiledId(tileIndex);
                    unsigned char viewOffset = configMgr->getViewOffsetByTiledId(tileIndex);
                    std::string tileGraphicSetName = "tiles/" + mapTileConfig->tileName;
                    const GraphicSet* tileGraphicSet = graphicsMgr->getGraphicSet(tileGraphicSetName);

                    std::array<const Animation*, 4> tileAnimations = {
                        tileGraphicSet->getByView((FourDirectionsView("south") + viewOffset).getViewName()),
                        tileGraphicSet->getByView((FourDirectionsView("east") + viewOffset).getViewName()),
                        tileGraphicSet->getByView((FourDirectionsView("north") + viewOffset).getViewName()),
                        tileGraphicSet->getByView((FourDirectionsView("west") + viewOffset).getViewName())
                    };

                    mapTile->setTile(mapTileConfig, tileAnimations);
                    mapTile->isle = isle;
                }
            }
        }

            // Startpunkt: Diesen Punkt wollen wir auf den Bildschirm zentrieren
        else if (strcmp(nodeType, "startpoint") == 0) {
            map->setMapCoordsCentered(mapCoords);
        }
    }
    map->screenZoom = 1;
    map->screenView = FourDirectionsView("south");
}

void GameIO::loadColonies(Game* game, rapidxml::xml_node<>* objectgroupColoniesNode) {
    Map* map = game->getMap();

    for (rapidxml::xml_node<>* objectNode = objectgroupColoniesNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();
        if (strcmp(nodeType, "colony") != 0) {
            continue;
        }

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);

        rapidxml::xml_node<>* propertiesNode = objectNode->first_node("properties", 10, true);
        int playerNr = atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
        Player* player = game->getPlayer(playerNr - 1);
        MapTile* mapTile = map->mapTiles->getData(mapCoords.x(), mapCoords.y(), nullptr);

        // Siedlung anlegen
        Colony* colony = game->foundNewColony(player, mapTile->isle);

        // Waren
        colony->getGoods(GoodsType::WOOL).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_wool"));
        colony->getGoods(GoodsType::CATTLE).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_cattle"));
        colony->getGoods(GoodsType::FOOD).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_food"));
        colony->getGoods(GoodsType::CLOTH).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_cloth"));
        colony->getGoods(GoodsType::TOOLS).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_tools"));
        colony->getGoods(GoodsType::WOOD).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_wood"));
        colony->getGoods(GoodsType::BRICKS).inventory =
            atoi(getPropertyValueFromPropertiesNode(propertiesNode, "goods_bricks"));
    }
}

void GameIO::loadStructures(Game* game, rapidxml::xml_node<>* objectgroupStructuresNode) {
    for (rapidxml::xml_node<>* objectNode = objectgroupStructuresNode->first_node("object", 6, true); objectNode != nullptr;
         objectNode = objectNode->next_sibling("object", 6, true)) {

        // Gucken, ob wir den Knoten-Typ kennen
        const char* nodeType = objectNode->first_attribute("type", 4, true)->value();
        if (strcmp(nodeType, "structure") != 0 && strcmp(nodeType, "building") != 0) {
            continue;
        }

        // Allgemeine Daten einlesen
        const char* nodeNameValue = objectNode->first_attribute("name", 4, true)->value();

        MapCoords mapCoords = getMapCoordsFromObjectNode(objectNode);

        rapidxml::xml_node<>* propertiesNode = objectNode->first_node("properties", 10, true);
        int playerNr = atoi(getPropertyValueFromPropertiesNode(propertiesNode, "player"));
        Player* player = game->getPlayer(playerNr - 1);

        // Konkreten Struktur-Typ finden und MapObject anlegen
        StructureType structureType;
        if (strcmp(nodeType, "structure") == 0 || strcmp(nodeType, "building") == 0) {
            if (strcmp(nodeNameValue, "street") == 0) {
                structureType = StructureType::STREET;
            } else if (strcmp(nodeNameValue, "chapel") == 0) {
                structureType = StructureType::CHAPEL;
            } else if (strcmp(nodeNameValue, "pioneers_house1") == 0) {
                structureType = StructureType::PIONEERS_HOUSE1;
            } else if (strcmp(nodeNameValue, "stonemason") == 0) {
                structureType = StructureType::STONEMASON;
            } else if (strcmp(nodeNameValue, "office1") == 0) {
                structureType = StructureType::OFFICE1;
            } else if (strcmp(nodeNameValue, "marketplace") == 0) {
                structureType = StructureType::MARKETPLACE;
            } else if (strcmp(nodeNameValue, "foresters") == 0) {
                structureType = StructureType::FORESTERS;
            } else if (strcmp(nodeNameValue, "sheep_farm") == 0) {
                structureType = StructureType::SHEEP_FARM;
            } else if (strcmp(nodeNameValue, "weaving_mill1") == 0) {
                structureType = StructureType::WEAVING_MILL1;
            } else {
                throw new std::runtime_error("Illegal structure or building name");
            }

            // TODO Ansicht in die TMX legen und dort wieder rauslesen
            FourDirectionsView view;
            game->addStructure(mapCoords, structureType, view, player);
        }
    }
}


MapCoords GameIO::getMapCoordsFromObjectNode(rapidxml::xml_node<>* objectNode) {
    int x = atoi(objectNode->first_attribute("x", 1, true)->value());
    int y = atoi(objectNode->first_attribute("y", 1, true)->value());

    int mapX = x / IGraphicsMgr::TILE_HEIGHT; // tiled rechnet merkwürdigerweise auch für X in KachelHÖHE
    int mapY = y / IGraphicsMgr::TILE_HEIGHT;

    return MapCoords(mapX, mapY);
}

const char* GameIO::getPropertyValueFromPropertiesNode(rapidxml::xml_node<>* propertiesNode, const char* propertyName) {
    for (rapidxml::xml_node<>* propertyNode = propertiesNode->first_node("property", 8, true); propertyNode != nullptr;
         propertyNode = propertyNode->next_sibling("property", 8, true)) {

        const char* thisPropertyName = propertyNode->first_attribute("name", 4, true)->value();
        if (strcmp(thisPropertyName, propertyName) != 0) {
            continue;
        }

        const char* propertyValue = propertyNode->first_attribute("value", 5, true)->value();
        return propertyValue;
    }

    return nullptr;
}