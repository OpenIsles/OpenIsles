#include <cstring>
#include <iostream>
#include <stdexcept>
#include "config/ConfigMgr.h"


ConfigMgr::ConfigMgr() {
    loadBuildingConfig();
    loadTilesConfig();
}

ConfigMgr::~ConfigMgr() {
    for(int i = 0; i < StructureType::MAX_STRUCTURE; i++) {
        if (buildingConfigs[i] != nullptr) {
            delete buildingConfigs[i];
        }
    }
    delete[] buildingConfigs;

    for(int i = 0; i < 128; i++) {
        if (mapTileConfigs[i] != nullptr) {
            delete mapTileConfigs[i];
        }
    }
    delete[] mapTileConfigs;
}

void ConfigMgr::loadBuildingConfig() {
    buildingConfigs = new BuildingConfig*[StructureType::MAX_STRUCTURE];
    memset(buildingConfigs, 0, StructureType::MAX_STRUCTURE * sizeof(BuildingConfig*));

    for (int i = StructureType::STREET; i <= StructureType::STREET_CROSS; i++) {
        buildingConfigs[i] = new BuildingConfig();
        buildingConfigs[i]->name = "Pflasterstraße";
        buildingConfigs[i]->catchmentArea = nullptr;
        buildingConfigs[i]->buildingCosts = { 5, 0, 0, 0 };
        buildingConfigs[i]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };
    }

    buildingConfigs[StructureType::CHAPEL] = new BuildingConfig();
    buildingConfigs[StructureType::CHAPEL]->name = "Kapelle";
    buildingConfigs[StructureType::CHAPEL]->catchmentArea = new RectangleData<char>(18, 17);
    memcpy(buildingConfigs[StructureType::CHAPEL]->catchmentArea->data, "000000111111000000000011111111110000000111111111111000001111111111111100011111111111111110011111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111110011111111111111110001111111111111100000111111111111000000011111111110000000000111111000000", 306);
    buildingConfigs[StructureType::CHAPEL]->buildingCosts = { 100, 2, 5, 0 };
    buildingConfigs[StructureType::CHAPEL]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };

    buildingConfigs[StructureType::PIONEERS_HOUSE1] = new BuildingConfig();
    buildingConfigs[StructureType::PIONEERS_HOUSE1]->name = "Haus (Pioniere)";
    buildingConfigs[StructureType::PIONEERS_HOUSE1]->catchmentArea = new RectangleData<char>(14, 14);
    memcpy(buildingConfigs[StructureType::PIONEERS_HOUSE1]->catchmentArea->data, "0000111111000000011111111000001111111111000111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111101111111111110001111111111000001111111100000001111110000", 196);
    buildingConfigs[StructureType::PIONEERS_HOUSE1]->buildingCosts = { 0, 0, 3, 0 };
    buildingConfigs[StructureType::PIONEERS_HOUSE1]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };

    buildingConfigs[StructureType::SIGNALFIRE] = new BuildingConfig();
    buildingConfigs[StructureType::SIGNALFIRE]->name = "Signalturm";
    buildingConfigs[StructureType::SIGNALFIRE]->catchmentArea = new RectangleData<char>(5, 5);
    memcpy(buildingConfigs[StructureType::SIGNALFIRE]->catchmentArea->data, "0010001110111110111000100", 25);
    buildingConfigs[StructureType::SIGNALFIRE]->buildingCosts = { 50, 1, 7, 0 };
    buildingConfigs[StructureType::SIGNALFIRE]->buildingProduction = {
        GoodsSlot(GoodsType::BRICKS, 5),
        GoodsSlot(GoodsType::TOOLS, 10),
        GoodsSlot(GoodsType::WOOD, 3)
    };
    buildingConfigs[StructureType::SIGNALFIRE]->productionRate = 1.5;
    buildingConfigs[StructureType::SIGNALFIRE]->inputConsumptionRate = 1.5;
    buildingConfigs[StructureType::SIGNALFIRE]->input2ConsumptionRate = 1.5;

    buildingConfigs[StructureType::HERBARY] = new BuildingConfig();
    buildingConfigs[StructureType::HERBARY]->name = "Kräuterfeld";
    buildingConfigs[StructureType::HERBARY]->catchmentArea = new RectangleData<char>(11, 11);
    memcpy(buildingConfigs[StructureType::HERBARY]->catchmentArea->data, "0000111000000011111000011111111100111111111011111111111111111111111111111111101111111110011111111100001111100000001110000", 121);
    buildingConfigs[StructureType::HERBARY]->buildingCosts = { 100, 0, 0, 0 };
    buildingConfigs[StructureType::HERBARY]->buildingProduction = {
        GoodsSlot(GoodsType::TOOLS, 4),
        GoodsSlot(GoodsType::BRICKS, 4),
        GoodsSlot()
    };
    buildingConfigs[StructureType::HERBARY]->productionRate = 2.4;
    buildingConfigs[StructureType::HERBARY]->inputConsumptionRate = 3.6;

    buildingConfigs[StructureType::BRICKYARD] = new BuildingConfig();
    buildingConfigs[StructureType::BRICKYARD]->name = "Steinbruch";
    buildingConfigs[StructureType::BRICKYARD]->catchmentArea = new RectangleData<char>(10, 6);
    memcpy(buildingConfigs[StructureType::BRICKYARD]->catchmentArea->data, "001111110001111111101111111111111111111101111111100011111100", 60);
    buildingConfigs[StructureType::BRICKYARD]->buildingCosts = { 100, 5, 5, 0 };
    buildingConfigs[StructureType::BRICKYARD]->buildingProduction = {
        GoodsSlot(GoodsType::BRICKS, 10),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::BRICKYARD]->productionRate = 0.7;

    buildingConfigs[StructureType::BRICKYARD2] = new BuildingConfig();
    buildingConfigs[StructureType::BRICKYARD2]->name = "Steinbruch (gedreht)";
    buildingConfigs[StructureType::BRICKYARD2]->catchmentArea = new RectangleData<char>(6, 10);
    memcpy(buildingConfigs[StructureType::BRICKYARD2]->catchmentArea->data, "001100011110111111111111111111111111111111111111011110001100", 60);
    buildingConfigs[StructureType::BRICKYARD2]->buildingCosts = { 100, 5, 5, 0 };
    buildingConfigs[StructureType::BRICKYARD2]->buildingProduction = {
        GoodsSlot(GoodsType::BRICKS, 10),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::BRICKYARD2]->productionRate = 0.7;

    buildingConfigs[StructureType::OFFICE1] = new BuildingConfig();
    buildingConfigs[StructureType::OFFICE1]->name = "Kontor";
    buildingConfigs[StructureType::OFFICE1]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[StructureType::OFFICE1]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[StructureType::OFFICE1]->buildingCosts = { 100, 3, 6, 0 };
    buildingConfigs[StructureType::OFFICE1]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };

    buildingConfigs[StructureType::MARKETPLACE] = new BuildingConfig();
    buildingConfigs[StructureType::MARKETPLACE]->name = "Marktplatz";
    buildingConfigs[StructureType::MARKETPLACE]->catchmentArea = new RectangleData<char>(38, 38);
    memcpy(buildingConfigs[StructureType::MARKETPLACE]->catchmentArea->data, "0000000000000001111111100000000000000000000000000111111111111111100000000000000000000111111111111111111110000000000000000011111111111111111111110000000000000011111111111111111111111111000000000001111111111111111111111111111000000000111111111111111111111111111111000000001111111111111111111111111111110000000111111111111111111111111111111110000011111111111111111111111111111111110000111111111111111111111111111111111100011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111111100111111111111111111111111111111111111001111111111111111111111111111111111110011111111111111111111111111111111111100011111111111111111111111111111111110000111111111111111111111111111111111100000111111111111111111111111111111110000000111111111111111111111111111111000000001111111111111111111111111111110000000001111111111111111111111111111000000000001111111111111111111111111100000000000000111111111111111111111100000000000000000111111111111111111110000000000000000000011111111111111110000000000000000000000000011111111000000000000000", 1444);
    buildingConfigs[StructureType::MARKETPLACE]->buildingCosts = { 200, 4, 10, 0 };
    buildingConfigs[StructureType::MARKETPLACE]->buildingProduction = { GoodsSlot(), GoodsSlot(), GoodsSlot() };

    buildingConfigs[StructureType::FORESTERS] = new BuildingConfig();
    buildingConfigs[StructureType::FORESTERS]->name = "Forsthaus";
    buildingConfigs[StructureType::FORESTERS]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(buildingConfigs[StructureType::FORESTERS]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    buildingConfigs[StructureType::FORESTERS]->buildingCosts = { 50, 2, 0, 0 };
    buildingConfigs[StructureType::FORESTERS]->buildingProduction = {
        GoodsSlot(GoodsType::WOOD, 10),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::FORESTERS]->productionRate = 3.5;

    buildingConfigs[StructureType::SHEEP_FARM] = new BuildingConfig();
    buildingConfigs[StructureType::SHEEP_FARM]->name = "Schaffarm";
    buildingConfigs[StructureType::SHEEP_FARM]->catchmentArea = new RectangleData<char>(8, 8);
    memcpy(buildingConfigs[StructureType::SHEEP_FARM]->catchmentArea->data, "0011110001111110111111111111111111111111111111110111111000111100", 64);
    buildingConfigs[StructureType::SHEEP_FARM]->buildingCosts = { 200, 2, 4, 0 };
    buildingConfigs[StructureType::SHEEP_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::WOOL, 3),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::SHEEP_FARM]->productionRate = 30;

    buildingConfigs[StructureType::WEAVING_MILL1] = new BuildingConfig();
    buildingConfigs[StructureType::WEAVING_MILL1]->name = "Webstube";
    buildingConfigs[StructureType::WEAVING_MILL1]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(buildingConfigs[StructureType::WEAVING_MILL1]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    buildingConfigs[StructureType::WEAVING_MILL1]->buildingCosts = { 200, 3, 6, 0 };
    buildingConfigs[StructureType::WEAVING_MILL1]->buildingProduction = {
        GoodsSlot(GoodsType::CLOTH, 7),
        GoodsSlot(GoodsType::WOOL, 7),
        GoodsSlot()
    };
    buildingConfigs[StructureType::WEAVING_MILL1]->productionRate = 10.0;
    buildingConfigs[StructureType::WEAVING_MILL1]->inputConsumptionRate = 5.0;

    buildingConfigs[StructureType::CATTLE_FARM] = new BuildingConfig();
    buildingConfigs[StructureType::CATTLE_FARM]->name = "Rinderfarm";
    buildingConfigs[StructureType::CATTLE_FARM]->catchmentArea = new RectangleData<char>(6, 6);
    memcpy(buildingConfigs[StructureType::CATTLE_FARM]->catchmentArea->data, "011110111111111111111111111111011110", 36);
    buildingConfigs[StructureType::CATTLE_FARM]->buildingCosts = { 100, 1, 4, 0 };
    buildingConfigs[StructureType::CATTLE_FARM]->buildingProduction = {
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::CATTLE_FARM]->productionRate = 7.5;

    buildingConfigs[StructureType::BUTCHERS] = new BuildingConfig();
    buildingConfigs[StructureType::BUTCHERS]->name = "Fleischerei";
    buildingConfigs[StructureType::BUTCHERS]->catchmentArea = new RectangleData<char>(22, 22);
    memcpy(buildingConfigs[StructureType::BUTCHERS]->catchmentArea->data, "0000000111111110000000000001111111111110000000001111111111111100000001111111111111111000001111111111111111110001111111111111111111100111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111110011111111111111111111000111111111111111111000001111111111111111000000011111111111111000000000111111111111000000000000111111110000000", 484);
    buildingConfigs[StructureType::BUTCHERS]->buildingCosts = { 150, 3, 6, 0 };
    buildingConfigs[StructureType::BUTCHERS]->buildingProduction = {
        GoodsSlot(GoodsType::FOOD, 4),
        GoodsSlot(GoodsType::CATTLE, 4),
        GoodsSlot()
    };
    buildingConfigs[StructureType::BUTCHERS]->productionRate = 5;
    buildingConfigs[StructureType::BUTCHERS]->inputConsumptionRate = 10;

    buildingConfigs[StructureType::TOOLSMITHS] = new BuildingConfig();
    buildingConfigs[StructureType::TOOLSMITHS]->name = "Werkzeugschmiede";
    buildingConfigs[StructureType::TOOLSMITHS]->catchmentArea = new RectangleData<char>(32, 32);
    memcpy(buildingConfigs[StructureType::TOOLSMITHS]->catchmentArea->data, "0000000000001111111100000000000000000000011111111111111000000000000000011111111111111111100000000000001111111111111111111100000000000111111111111111111111100000000011111111111111111111111100000001111111111111111111111111100000111111111111111111111111111100001111111111111111111111111111000111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110001111111111111111111111111111000011111111111111111111111111110000011111111111111111111111111000000011111111111111111111111100000000011111111111111111111110000000000011111111111111111111000000000000011111111111111111100000000000000001111111111111100000000000000000000011111111000000000000", 1024);
    buildingConfigs[StructureType::TOOLSMITHS]->buildingCosts = { 150, 3, 2, 5 };
    buildingConfigs[StructureType::TOOLSMITHS]->buildingProduction = {
        GoodsSlot(GoodsType::TOOLS, 5),
        GoodsSlot(),
        GoodsSlot()
    };
    buildingConfigs[StructureType::TOOLSMITHS]->productionRate = 2.5;
}

void ConfigMgr::loadTilesConfig() {
    // TODO Fehlermanagement, wenn die XML-Datei mal nicht so hübsch aussieht, dass alle Tags da sind

    // nearest pixel sampling für die Kacheln, damit die fließend ineinander übergehen
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    mapTileConfigs = new MapTileConfig*[128];
    memset(mapTileConfigs, 0, 128 * sizeof(MapTileConfig*));

    // Datei öffnen
    rapidxml::file<> xmlFile("data/config/tiles.xml");

    rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
    xmlDocument->parse<0>(xmlFile.data());

    // Tiles auslesen
    rapidxml::xml_node<>* tilesConfigNode = xmlDocument->first_node("tiles-config", 12, true);
    rapidxml::xml_node<>* tilesNode = tilesConfigNode->first_node("tiles", 5, true);
    for (rapidxml::xml_node<>* tileNode = tilesNode->first_node("tile", 4, true); tileNode != nullptr;
         tileNode = tileNode->next_sibling("tile", 4, true)) {

        int tileIndex = atoi(tileNode->first_attribute("tile-index", 10, true)->value());
        const char* tileGraphicFile = tileNode->first_attribute("graphic-file", 12, true)->value();
        bool tileIsOcean = xmlAttributeToBool(tileNode->first_attribute("ocean", 5, true), false);
        bool tileIsWalkableAndBuildable =
            xmlAttributeToBool(tileNode->first_attribute("walkable-and-buildable", 22, true), false);

        // Dateiname abkopieren. Strings aus der XML werden ungültig, wenn wir mit der Datei fertig sind
        char* tileGraphicFileCopy = new char[strlen(tileGraphicFile) + 1];
        strcpy(tileGraphicFileCopy, tileGraphicFile);

        mapTileConfigs[tileIndex] = new MapTileConfig();
        mapTileConfigs[tileIndex]->graphicsFile = tileGraphicFileCopy;
        mapTileConfigs[tileIndex]->isOcean = tileIsOcean;
        mapTileConfigs[tileIndex]->isWalkableAndBuildable = tileIsWalkableAndBuildable;
    }
}

bool ConfigMgr::xmlAttributeToBool(rapidxml::xml_attribute<>* attribute, bool defaultValue) {
    // Attribut nicht da? Default-Value verwenden
    if (attribute == nullptr) {
        return defaultValue;
    }

    const char* value = attribute->value();
    if (strcmp(value, "false") == 0) {
        return false;
    }
    else if (strcmp(value, "true") == 0) {
        return true;
    }
    else {
        std::cerr << "Illegal bool value '" << value << "'." << std::endl;
        throw new std::runtime_error("Illegal bool value");
    }
}
