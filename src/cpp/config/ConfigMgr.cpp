#include <cstring>
#include "global.h"
#include "config/ConfigMgr.h"
#include "config/ErrorInConfigException.h"
#include "config/Good.h"
#include "map/MapObjectType.h"
#include "utils/StringFormat.h"


/**
 * @brief Da sich MapObjectType und PopulationTier gegenseitig referenzieren, müssen wir uns beim Laden
 * für eine Reihenfolge entscheiden und die endgültige Zuordnung später machen. Diese Map speichert zwischen,
 * welcher MapObjectType (Key der Map) später welchen PopulationTier (Value der Map = populationTier.name)
 * zugeordnet kriegt.
 */
static std::unordered_map<MapObjectType*, std::string> mapObjectTypeToPopulationTierName;


ConfigMgr::ConfigMgr(std::string configPath) {
    try {
        loadGoods(configPath + "/goods.xml");
        Log::info(_("Loaded goods."));

        loadCarrierMapObjectTypes(configPath + "/carriers.xml");
        Log::info(_("Loaded carrier mapObjectTypes."));

        loadMapObjectTypes(configPath + "/map-objects.xml");
        Log::info(_("Loaded mapObjectTypes."));

        loadPopulationTiers(configPath + "/population-tiers.xml");
        Log::info(_("Loaded population tiers."));

        loadTilesConfig(configPath + "/tiles.xml");
        Log::info(_("Loaded tiles."));

    } catch (const rapidxml::parse_error& e) {
        // Fehler beim Parsen einer XML.
        // TODO e.what() kommt immer auf Englisch zurück. Das muss übersetzt werden. Idee: künstlich Phrasen anlegen

        throw ErrorInConfigException(string_sprintf(_("Error while loading the configuration: %s."), e.what()));
    }
}

ConfigMgr::~ConfigMgr() {
}

void ConfigMgr::loadGoods(const std::string& configFilePath) {
    rapidxml::file<> xmlFile(configFilePath.c_str());

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* goodsNode = xmlDocument->first_node("goods", 5, true);

    for (rapidxml::xml_node<>* goodNode = goodsNode->first_node("good", 4, true); goodNode != nullptr;
         goodNode = goodNode->next_sibling("good", 4, true)) {

        std::string name = goodNode->first_attribute("name", 4, true)->value();
        bool rawMaterial = xmlAttributeToBool(goodNode->first_attribute("raw-material", 12, true), false);
        bool invisible = xmlAttributeToBool(goodNode->first_attribute("invisible", 9, true), false);

        Good& good = goodsMap[name];
        good.name = name;
        good.rawMaterial = rawMaterial;
        good.invisible = invisible;

        goodsList.push_back(&good);
    }

    delete xmlDocument;
}

void ConfigMgr::loadMapObjectTypes(const std::string& configFilePath) {
    rapidxml::file<> xmlFile(configFilePath.c_str());

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* mapObjectsNode = xmlDocument->first_node("map-objects", 11, true);

    for (rapidxml::xml_node<>* node = mapObjectsNode->first_node(); node != nullptr; node = node->next_sibling()) {
        std::string name = std::string(node->first_attribute("name", 4, true)->value());
        MapObjectType& mapObjectType = mapObjectTypesMap[name];
        mapObjectType.name = name;

        // Knoten-Typ
        const char* nodeName = node->name();
        if (std::strcmp(nodeName, "harvestable") == 0) {
            mapObjectType.type = MapObjectTypeClass::HARVESTABLE;
        } else if (std::strcmp(nodeName, "structure") == 0) {
            mapObjectType.type = MapObjectTypeClass::STRUCTURE;
        } else if (std::strcmp(nodeName, "street") == 0) {
            mapObjectType.type = MapObjectTypeClass::STREET;
        } else if (std::strcmp(nodeName, "building") == 0) {
            mapObjectType.type = MapObjectTypeClass::BUILDING;
        } else {
            throw ErrorInConfigException(string_sprintf(_("Illegal node '%s'."), nodeName));
        }

        // Basics
        mapObjectType.mapWidth = (unsigned char) stringToUnsignedLong(
            node->first_attribute("width", 5, true)->value());
        mapObjectType.mapHeight = (unsigned char) stringToUnsignedLong(
            node->first_attribute("height", 6, true)->value());

        rapidxml::xml_node<>* graphicSetNode = node->first_node("graphic-set", 11, true);
        mapObjectType.graphicSetName = std::string(graphicSetNode->first_attribute("name", 4, true)->value());

        // TODO <animation>-Unter-Tag auswerten und für die Animationen nutzen

        // Structure-Placing
        const char* structurePlacing = node->first_node("structure-placing", 17, true)->value();
        if (std::strcmp(structurePlacing, "individually") == 0) {
            mapObjectType.structurePlacing = StructurePlacing::INDIVIDUALLY;
        } else if (std::strcmp(structurePlacing, "rectangle") == 0) {
            mapObjectType.structurePlacing = StructurePlacing::RECTANGLE;
        } else if (std::strcmp(structurePlacing, "path") == 0) {
            mapObjectType.structurePlacing = StructurePlacing::PATH;
        } else {
            throw ErrorInConfigException(
                string_sprintf(_("Illegal value '%s' for structurePlacing."), structurePlacing));
        }

        // Baubar auf
        MapTileTypeInt placeableOnMapTileTypeMask = 0;
        rapidxml::xml_node<>* placeableOnNode = node->first_node("placeable-on", 12, true);
        for (rapidxml::xml_node<>* mapTileTypeNode = placeableOnNode->first_node("map-tile-type", 13, true);
             mapTileTypeNode != nullptr;
             mapTileTypeNode = mapTileTypeNode->next_sibling("map-tile-type", 13, true)) {

            const char* typeString = mapTileTypeNode->value();
            MapTileType mapTileType = getMapTileTypeByName(typeString);

            placeableOnMapTileTypeMask |= (MapTileTypeInt) mapTileType;
        }
        mapObjectType.placeableOnMapTileTypeMask = placeableOnMapTileTypeMask;

        // Wald?
        rapidxml::xml_node<>* forestNode = node->first_node("forest", 6, true);
        mapObjectType.isForest = (forestNode != nullptr);
        // TODO das Tag können wir später erweitern, für welchen Inseltyp der Wald ist (nördlich, südlich)

        // Baukosten
        rapidxml::xml_node<>* buildingCostsNode = node->first_node("building-costs", 14, true);
        readBuildingCosts(mapObjectType.buildingCosts, buildingCostsNode);

        // optionale Tags...

        // Betriebskosten
        rapidxml::xml_node<>* operatingCostsNode = node->first_node("operating-costs", 15, true);
        if (operatingCostsNode != nullptr) {
            rapidxml::xml_node<>* runningNode = operatingCostsNode->first_node("running", 7, true);
            mapObjectType.operatingCosts.running = (unsigned char) stringToUnsignedLong(runningNode->value());

            rapidxml::xml_node<>* decommissionedNode = operatingCostsNode->first_node("decommissioned", 14, true);
            if (decommissionedNode != nullptr) {
                mapObjectType.operatingCosts.decommissioned =
                    (unsigned char) stringToUnsignedLong(decommissionedNode->value());
            } else {
                mapObjectType.operatingCosts.decommissioned = mapObjectType.operatingCosts.running;
            }
        }

        // Einzugsbereich
        rapidxml::xml_node<>* catchmentAreaNode = node->first_node("catchment-area", 14, true);
        if (catchmentAreaNode != nullptr) {
            const char* catchmentAreaValue = catchmentAreaNode->value();
            mapObjectType.catchmentArea.reset(parseCatchmentArea(catchmentAreaValue));

            rapidxml::xml_attribute<>* increasesBuildableAreaAttribute =
                catchmentAreaNode->first_attribute("increases-buildable-area", 24, true);
            if (increasesBuildableAreaAttribute != nullptr) {
                mapObjectType.increasesBuildableArea = true;
            }
        }

        // produzierte und verbrauchte Waren
        rapidxml::xml_node<>* productionSlotsNode = node->first_node("production-slots", 16, true);
        if (productionSlotsNode != nullptr) {
            rapidxml::xml_node<>* outputNode = productionSlotsNode->first_node("output", 6, true);
            readGoodSlotConfig(mapObjectType.buildingProduction.output, outputNode);

            if (outputNode != nullptr) {
                mapObjectType.secondsToProduce = (unsigned char) stringToUnsignedLong(
                    outputNode->first_attribute("seconds-to-produce", 18, true)->value());
            }

            rapidxml::xml_node<>* inputNode = productionSlotsNode->first_node("input", 5, true);
            if (inputNode != nullptr) {
                readGoodSlotConfig(mapObjectType.buildingProduction.input, inputNode);

                mapObjectType.inputAmountForProduction = (unsigned char) stringToUnsignedLong(
                    inputNode->first_attribute("amount-for-production", 21, true)->value());

                rapidxml::xml_node<>* input2Node = productionSlotsNode->first_node("input2", 6, true);
                if (input2Node != nullptr) {
                    readGoodSlotConfig(mapObjectType.buildingProduction.input2, input2Node);
                }
            }
        }

        // Lagergebäude? Kapazität
        rapidxml::xml_node<>* goodsCapacityNode = node->first_node("goods-capacity", 14, true);
        if (goodsCapacityNode != nullptr) {
            mapObjectType.goodsCapacity = (unsigned char) stringToUnsignedLong(goodsCapacityNode->value());
        }

        // Einwohner
        rapidxml::xml_node<>* inhabitantsNode = node->first_node("inhabitants", 11, true);
        if (inhabitantsNode != nullptr) {
            mapObjectType.inhabitants = (unsigned char) stringToUnsignedLong(inhabitantsNode->value());
        }

        // Häuser: Bevölkerungsgruppe
        rapidxml::xml_node<>* populationTierNode = node->first_node("population-tier", 15, true);
        if (populationTierNode != nullptr) {
            const char* populationTierString = populationTierNode->value();

            // für später den String zwischenspeichern. Zuordnung kann erst gemacht werden,
            // nachdem die PopulationTiers geladen sind.
            mapObjectTypeToPopulationTierName[&mapObjectType] = populationTierString;
        }

        // Träger
        rapidxml::xml_node<>* carriersNode = node->first_node("carriers", 8, true);
        if (carriersNode != nullptr) {
            std::string carrierMapObjectTypeName = carriersNode->first_attribute("name", 4, true)->value();
            const MapObjectType* carrierType = &mapObjectTypesMap.at(carrierMapObjectTypeName);

            mapObjectType.carrier.mapObjectType = carrierType;
            mapObjectType.maxCarriers = (unsigned char) stringToUnsignedLong(
                carriersNode->first_attribute("max-carriers", 12, true)->value());
        }

        // Harvestables: maximales Alter
        rapidxml::xml_node<>* maxAgeNode = node->first_node("max-age", 7, true);
        if (maxAgeNode != nullptr) {
            mapObjectType.maxAge = (unsigned char) stringToUnsignedLong(maxAgeNode->value());
        }

        Log::info(_("Loaded mapObjectType '%s'."), mapObjectType.name.c_str());
    }

    delete xmlDocument;
}

void ConfigMgr::loadCarrierMapObjectTypes(const std::string& configFilePath) {
    rapidxml::file<> xmlFile(configFilePath.c_str());

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* carriersNode = xmlDocument->first_node("carriers", 8, true);

    for (rapidxml::xml_node<>* node = carriersNode->first_node(); node != nullptr; node = node->next_sibling()) {
        std::string name = std::string(node->first_attribute("name", 4, true)->value());
        MapObjectType& mapObjectType = mapObjectTypesMap[name];
        mapObjectType.name = name;

        // Knoten-Typ
        const char* nodeName = node->name();
        if (std::strcmp(nodeName, "carrier") != 0) {
            throw ErrorInConfigException(string_sprintf(_("Illegal node '%s'."), nodeName));
        }

        mapObjectType.type = MapObjectTypeClass::CARRIER;

        // Basics
        rapidxml::xml_node<>* capacityNode = node->first_node("capacity", 8, true);
        mapObjectType.carrier.capacity = (unsigned char) stringToUnsignedLong(capacityNode->value());

        rapidxml::xml_node<>* secondsToProduceNode = node->first_node("seconds-to-produce", 18, true);
        if (secondsToProduceNode != nullptr) {
            mapObjectType.secondsToProduce = stringToDouble(secondsToProduceNode->value());
        }

        // TODO Animations

        Log::info(_("Loaded carrier mapObjectType '%s'."), mapObjectType.name.c_str());
    }

    delete xmlDocument;
}

void ConfigMgr::loadTilesConfig(const std::string& configFilePath) {
    // Datei öffnen
    rapidxml::file<> xmlFile(configFilePath.c_str());

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* tilesConfigNode = xmlDocument->first_node("tiles-config", 12, true);
    rapidxml::xml_node<>* tilesNode = tilesConfigNode->first_node("tiles", 5, true);

    // Tiles auslesen
    for (rapidxml::xml_node<>* tileNode = tilesNode->first_node("tile", 4, true); tileNode != nullptr;
         tileNode = tileNode->next_sibling("tile", 4, true)) {

        const char* tileName = tileNode->first_attribute("name", 4, true)->value();

        const char* mapTileTypeName = tileNode->first_attribute("type", 4, true)->value();
        const MapTileType mapTileType = getMapTileTypeByName(mapTileTypeName);

        MapTileConfig& mapTileConfig = mapTileConfigs[tileName];
        mapTileConfig.tileName = std::string(tileName);
        mapTileConfig.mapTileType = mapTileType;

        // TODO konfigurierbar machen
        if (mapTileConfig.tileName == "grass") {
            mapTileConfig.goodToHarvest = getGood("grass");
        }

        for (rapidxml::xml_node<>* tmxTileNode = tileNode->first_node("tmx-tile", 8, true); tmxTileNode != nullptr;
             tmxTileNode = tmxTileNode->next_sibling("tmx-tile", 8, true)) {

            int tmxTileIndex = stringToInteger(tmxTileNode->first_attribute("tmx-tile-index", 14, true)->value());
            const char* tileViewName = tmxTileNode->first_attribute("view", 4, true)->value();
            int xOffsetInTileset = stringToInteger(tmxTileNode->first_attribute("x", 1, true)->value());
            int yOffsetInTileset = stringToInteger(tmxTileNode->first_attribute("y", 1, true)->value());

            FourthDirection tileView = Direction::fromString(tileViewName);
            mapTileConfig.mapTileViewsOffsetXYInTileset[tileView] =
                std::make_pair(xOffsetInTileset, yOffsetInTileset);

            // Zuordnung zu Tiled-Kachel-ID und View-Offset merken
            tiledIdToMapTileConfig[tmxTileIndex] = &mapTileConfig;
            tiledIdToViewOffset[tmxTileIndex] = tileView / 2;
        }

        // Den Ozean separat merken
        // TODO Was, wenn mehrere Ozean-Kacheln in der tiles.xml stehen?
        if (mapTileType == MapTileType::OCEAN) {
            mapTileConfigOcean = &mapTileConfig;
        }
    }

    delete xmlDocument;
}

MapTileType ConfigMgr::getMapTileTypeByName(const std::string& mapTileTypeName) {
    if (mapTileTypeName == "ocean") {
        return MapTileType::OCEAN;
    }
    else if (mapTileTypeName == "fish-grounds") {
        return MapTileType::FISH_GROUNDS;
    }
    else if (mapTileTypeName == "shallow-water") {
        return MapTileType::SHALLOW_WATER;
    }
    else if (mapTileTypeName == "shore-ocean") {
        return MapTileType::SHORE_OCEAN;
    }
    else if (mapTileTypeName == "shore-grass") {
        return MapTileType::SHORE_GRASS;
    }
    else if (mapTileTypeName == "river") {
        return MapTileType::RIVER;
    }
    else if (mapTileTypeName == "mountain") {
        return MapTileType::MOUNTAIN;
    }
    else if (mapTileTypeName == "grass") {
        return MapTileType::GRASS;
    }
    else {
        throw ErrorInConfigException(string_sprintf(_("Illegal mapTileTypeName '%s'."), mapTileTypeName.c_str()));
    }
}

bool ConfigMgr::xmlAttributeToBool(rapidxml::xml_attribute<>* attribute, bool defaultValue) {
    // Attribut nicht da? Default-Value verwenden
    if (attribute == nullptr) {
        return defaultValue;
    }

    const char* value = attribute->value();
    if (std::strcmp(value, "false") == 0) {
        return false;
    }
    else if (std::strcmp(value, "true") == 0) {
        return true;
    }
    else {
        throw ErrorInConfigException(string_sprintf(_("Illegal bool value '%s'."), value));
    }
}

RectangleData<char>* ConfigMgr::parseCatchmentArea(const char* catchmentAreaValue) {
    // Ersetzt alle Vorkommen von `search` in `string` durch `replace`
    auto replaceString = [](std::string& string, const std::string& search, const std::string& replace) {
        std::string::size_type pos = 0;
        for (;;) {
            pos = string.find(search, pos);
            if (pos == std::string::npos) {
                return;
            }

            string.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    };

    // Step 1: Line-Endings normalisieren, damit nur einheitlich \n da sind
    std::string temp = std::string(catchmentAreaValue);
    replaceString(temp, "\r\n", "\n");
    replaceString(temp, "\r", "\n");
    catchmentAreaValue = temp.c_str();

    // Step 2: Durchgehen und width/height des Einzugsbereichs prüfen und ob alles zusammenpasst ///////////////////////
    bool notReachedFirstLine = true;
    int x = 0, y = 0; // 0-based Koordinaten, die wir grade vor uns haben
    int catchmentAreaWidth = 0;
    for (const char* ptr = catchmentAreaValue; *ptr != '\0'; ptr++) {
        if (notReachedFirstLine) {
            if (*ptr == '\n') {
                notReachedFirstLine = false;
            } else if (*ptr == ' ') {
                // Zeichen ok, ignorieren
            } else {
                throw ErrorInConfigException(_("Could not parse catchmentArea: Illegal char before first line."));
            }
            continue;
        }

        if (*ptr == ' ') {
            if (x == 0) {
                continue; // Leerzeichen am Zeilenanfang ok
            } else {
                throw ErrorInConfigException(_("Could not parse catchmentArea: Illegal space inside line."));
            }
        }

        if (*ptr == '0' || *ptr == '1') {
            x++;
        }

        if (*ptr == '\n') {
            // Zeile zu Ende? Wenn erste Zeile, gibt diese die Breite des Einzugsbereichs vor.
            // Jede weitere Zeile muss dieselbe Breite haben.
            if (y == 0) {
                catchmentAreaWidth = x;
            } else {
                if (x != catchmentAreaWidth) {
                    throw ErrorInConfigException(_("Could not parse catchmentArea: widths are not equal."));
                }
            }

            x = 0;
            y++;
        }
    }

    // Letzte Zeile muss leer sein.
    if (x != 0) {
        throw ErrorInConfigException(_("Could not parse catchmentArea: Last line was not completly empty."));
    }
    int catchmentAreaHeight = y;

    // Step 3: Nun nur durchgehen und die '1'/'0'-Zeichen aufnehmen
    RectangleData<char>* catchmentArea = new RectangleData<char>(catchmentAreaWidth, catchmentAreaHeight);

    int i = 0;
    for (const char* ptr = catchmentAreaValue; *ptr != '\0'; ptr++) {
        if (*ptr == '0' || *ptr == '1') {
            catchmentArea->data[i++] = *ptr;
        }
    }

    return catchmentArea;
}

void ConfigMgr::readGoodSlotConfig(GoodsSlot& goodSlot, rapidxml::xml_node<>* produtionSlotNode) {
    const char* goodName = produtionSlotNode->first_attribute("good", 4, true)->value();
    const Good* good = getGood(goodName);

    goodSlot.good = good;
    goodSlot.capacity = (unsigned int) stringToUnsignedLong(
        produtionSlotNode->first_attribute("capacity", 8, true)->value());
}

void ConfigMgr::loadPopulationTiers(const std::string& configFilePath) {
    rapidxml::file<> xmlFile(configFilePath.c_str());

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    rapidxml::xml_node<>* populationTiersNode = xmlDocument->first_node("population-tiers", 16, true);

    // Bevölkerungsgruppen
    unsigned char index = 1;
    for (rapidxml::xml_node<>* node = populationTiersNode->first_node("population-tier", 15, true);
         node != nullptr;
         node = node->next_sibling("population-tier", 15, true)) {

        PopulationTier populationTier;

        populationTier.index = index++;
        populationTier.name = std::string(node->first_attribute("name", 4, true)->value());

        // <advancement>
        rapidxml::xml_node<>* advancementNode = node->first_node("advancement", 11, true);

        rapidxml::xml_node<>* advancementMissingGoodsOkNode = advancementNode->first_node("missing-goods-ok", 16, true);
        if (advancementMissingGoodsOkNode != nullptr) {
            populationTier.advancementMissingGoodsOk = (unsigned char)
                stringToUnsignedLong(advancementMissingGoodsOkNode->value());
        }

        rapidxml::xml_node<>* advancementCostsNode = advancementNode->first_node("costs", 5, true);
        readBuildingCosts(populationTier.advancementCosts, advancementCostsNode);

        // <needs>
        rapidxml::xml_node<>* needsNode = node->first_node("needs", 5, true);
        if (needsNode != nullptr) {
            // <good>
            for (rapidxml::xml_node<>* goodNode = needsNode->first_node("good", 4, true);
                 goodNode != nullptr;
                 goodNode = goodNode->next_sibling("good", 4, true)) {

                std::string goodName = std::string(goodNode->first_attribute("name", 4, true)->value());

                NeededGood neededGood;
                neededGood.good = getGood(goodName);
                neededGood.consumePerCycle = stringToDouble(
                    goodNode->first_attribute("consume-per-cycle", 17, true)->value());

                populationTier.needsGoods.push_back(neededGood);
            }

            // <public-building>
            for (rapidxml::xml_node<>* publicBuildingNode = needsNode->first_node("public-building", 15, true);
                 publicBuildingNode != nullptr;
                 publicBuildingNode = publicBuildingNode->next_sibling("public-building", 15, true)) {

                std::string mapObjectTypeName =
                    std::string(publicBuildingNode->first_attribute("name", 4, true)->value());
                const MapObjectType* mapObjectType = getMapObjectType(mapObjectTypeName);

                populationTier.needsPublicBuildings.push_back(mapObjectType);
            }
        }

        // sonstige Tags
        populationTier.maxPopulationPerHouse = (unsigned char) stringToUnsignedLong(
            node->first_node("max-population-per-house", 24, true)->value());

        populationTier.taxesPerInhabitant = stringToDouble(
            node->first_node("taxes-per-inhabitant", 20, true)->value());

        populationTiers.insert(populationTier);

        Log::info(_("Loaded populationTier '%s'."), populationTier.name.c_str());
    }

    // Nahrungsbedürfnis
    rapidxml::xml_node<>* foodGoodNode = populationTiersNode->first_node("food-good", 9, true);

    std::string foodGoodName = std::string(foodGoodNode->first_attribute("name", 4, true)->value());

    foodGood.good = getGood(foodGoodName);
    foodGood.consumePerCycle = stringToDouble(
        foodGoodNode->first_attribute("consume-per-cycle", 17, true)->value());

    // XML-Datei schließen
    delete xmlDocument;

    // Jetzt sind alle PopulationTiers geladen. Wir ordnen diese nun den MapObjectTypes zu.
    for (auto iter = mapObjectTypeToPopulationTierName.cbegin();
         iter != mapObjectTypeToPopulationTierName.cend();
         iter++) {

        MapObjectType* mapObjectType = iter->first;
        const std::string& populationTierString = iter->second;

        const PopulationTier* populationTier = getPopulationTier(populationTierString);
        if (populationTier == nullptr) {
            throw ErrorInConfigException(string_sprintf(
                _("Illegal value '%s' for populationTier."), populationTierString.c_str()));
        }

        mapObjectType->populationTier = populationTier;
    }

    mapObjectTypeToPopulationTierName.clear(); // Daten wegräumen, brauchen wir nicht mehr
}

void ConfigMgr::readBuildingCosts(BuildingCosts& buildingCosts, rapidxml::xml_node<>* buildingCostsNode) {
    buildingCosts.coins = (unsigned int) stringToUnsignedLong(
        buildingCostsNode->first_node("coins", 5, true)->value());
    buildingCosts.tools = (unsigned int) stringToUnsignedLong(
        buildingCostsNode->first_node("tools", 5, true)->value());
    buildingCosts.wood = (unsigned int) stringToUnsignedLong(
        buildingCostsNode->first_node("wood", 4, true)->value());
    buildingCosts.bricks = (unsigned int) stringToUnsignedLong(
        buildingCostsNode->first_node("bricks", 6, true)->value());
}
