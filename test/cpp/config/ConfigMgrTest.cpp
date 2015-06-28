#include <cstring>
#include <gtest/gtest.h>
#include "config/ConfigMgr.h"
#include "config/MapTileType.h"
#include "map/MapObjectType.h"


/**
 * @brief Test, ob die Konfiguration der Map-Objekt-Typen korrekt geladen wird.
 */
TEST(ConfigMgrTest, loadMapObjectTypes) {
    ConfigMgr configMgr;

    // Teste 2 Objekte komplett

    const MapObjectType* cathedral = configMgr.getMapObjectType("cathedral");
    ASSERT_TRUE(cathedral != nullptr);
    ASSERT_EQ(MapObjectTypeClass::BUILDING, cathedral->type);
    ASSERT_EQ("cathedral", cathedral->name);
    ASSERT_EQ("Kathedrale", cathedral->title);
    ASSERT_EQ(6, cathedral->mapWidth);
    ASSERT_EQ(4, cathedral->mapHeight);
    ASSERT_EQ(StructurePlacing::INDIVIDUALLY, cathedral->structurePlacing);
    ASSERT_EQ((MapTileTypeInt) MapTileType::GRASS, cathedral->placeableOnMapTileTypeMask);
    ASSERT_EQ(50, cathedral->catchmentArea->width);
    ASSERT_EQ(50, cathedral->catchmentArea->height);
    ASSERT_EQ(0, memcmp("0000000000000000000011111111110000000000000000000000000000000000001111111111111111110000000000000000000000000000001111111111111111111111000000000000000000000000001111111111111111111111111100000000000000000000000111111111111111111111111111100000000000000000000111111111111111111111111111111110000000000000000011111111111111111111111111111111110000000000000001111111111111111111111111111111111110000000000000111111111111111111111111111111111111110000000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000011111111111111111111111111111111111111111100000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111111000011111111111111111111111111111111111111111111110001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111000111111111111111111111111111111111111111111111100001111111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000000011111111111111111111111111111111111111111100000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000000111111111111111111111111111111111111110000000000000111111111111111111111111111111111111000000000000000111111111111111111111111111111111100000000000000000111111111111111111111111111111110000000000000000000011111111111111111111111111110000000000000000000000011111111111111111111111111000000000000000000000000001111111111111111111111000000000000000000000000000000111111111111111111000000000000000000000000000000000000111111111100000000000000000000",
                        cathedral->catchmentArea->data, 2500));
    ASSERT_EQ(7500, cathedral->buildingCosts.coins);
    ASSERT_EQ(25, cathedral->buildingCosts.tools);
    ASSERT_EQ(25, cathedral->buildingCosts.wood);
    ASSERT_EQ(70, cathedral->buildingCosts.bricks);
    ASSERT_EQ(false, cathedral->buildingProduction.output.isUsed());
    ASSERT_EQ(false, cathedral->buildingProduction.input.isUsed());
    ASSERT_EQ(false, cathedral->buildingProduction.input2.isUsed());
    ASSERT_EQ(5, cathedral->inhabitants);
    ASSERT_EQ(0, cathedral->maxAge);

    const MapObjectType* sugarcaneField = configMgr.getMapObjectType("sugarcane-field");
    ASSERT_TRUE(sugarcaneField != nullptr);
    ASSERT_EQ(MapObjectTypeClass::HARVESTABLE, sugarcaneField->type);
    ASSERT_EQ("sugarcane-field", sugarcaneField->name);
    ASSERT_EQ("Zuckerrohrfeld", sugarcaneField->title);
    ASSERT_EQ(1, sugarcaneField->mapWidth);
    ASSERT_EQ(1, sugarcaneField->mapHeight);
    ASSERT_EQ(StructurePlacing::RECTANGLE, sugarcaneField->structurePlacing);
    ASSERT_EQ((MapTileTypeInt) MapTileType::GRASS, sugarcaneField->placeableOnMapTileTypeMask);
    ASSERT_TRUE(sugarcaneField->catchmentArea == nullptr);
    ASSERT_EQ(5, sugarcaneField->buildingCosts.coins);
    ASSERT_EQ(0, sugarcaneField->buildingCosts.tools);
    ASSERT_EQ(0, sugarcaneField->buildingCosts.wood);
    ASSERT_EQ(0, sugarcaneField->buildingCosts.bricks);
    ASSERT_EQ(false, sugarcaneField->buildingProduction.output.isUsed());
    ASSERT_EQ(false, sugarcaneField->buildingProduction.input.isUsed());
    ASSERT_EQ(false, sugarcaneField->buildingProduction.input2.isUsed());
    ASSERT_EQ(0, sugarcaneField->inhabitants);
    ASSERT_EQ(4, sugarcaneField->maxAge);

    // Teste sonstige Property-Inhalte, die obig nicht abgedeckt wurden

    const MapObjectType* cobbledStreetTee90 = configMgr.getMapObjectType("cobbled-street-tee90");
    ASSERT_EQ(MapObjectTypeClass::STRUCTURE, cobbledStreetTee90->type);
    ASSERT_EQ(StructurePlacing::PATH, cobbledStreetTee90->structurePlacing);

    const MapObjectType* toolsmiths = configMgr.getMapObjectType("toolsmiths");
    ASSERT_EQ(150, toolsmiths->buildingCosts.coins);
    ASSERT_EQ(3, toolsmiths->buildingCosts.tools);
    ASSERT_EQ(2, toolsmiths->buildingCosts.wood);
    ASSERT_EQ(5, toolsmiths->buildingCosts.bricks);

    const MapObjectType* tavern = configMgr.getMapObjectType("tavern");
    ASSERT_EQ(26, tavern->catchmentArea->width);
    ASSERT_EQ(25, tavern->catchmentArea->height);
    ASSERT_EQ(0, memcmp("00000000011111111000000000000000011111111111100000000000011111111111111110000000001111111111111111110000000111111111111111111110000011111111111111111111110000111111111111111111111100011111111111111111111111100111111111111111111111111011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111100111111111111111111111111000111111111111111111111100001111111111111111111111000001111111111111111111100000001111111111111111110000000001111111111111111000000000000111111111111000000000000000011111111000000000",
                        tavern->catchmentArea->data, 650));

    const MapObjectType* distillery = configMgr.getMapObjectType("distillery");
    ASSERT_EQ(true, distillery->buildingProduction.output.isUsed());
    ASSERT_EQ(true, distillery->buildingProduction.input.isUsed());
    ASSERT_EQ("alcohol", distillery->buildingProduction.output.good->name);
    ASSERT_EQ(4, distillery->buildingProduction.output.capacity);
    ASSERT_EQ(2.5, distillery->buildingProduction.output.rate);
    ASSERT_EQ("sugar", distillery->buildingProduction.input.good->name);
    ASSERT_EQ(6, distillery->buildingProduction.input.capacity);
    ASSERT_EQ(5.0, distillery->buildingProduction.input.rate);
    // TODO Testcase für output, input UND input2 gesetzt; aktuell haben wir kein solches Gebäude
    // TODO Testcase für kombinierte Bitmaske in placeableOnMapTileTypeMask
}


/**
 * @brief Testet, ob das Parsen des Einzugsbereichs aus der XML korrekt funktioniert.
 */
TEST(ConfigMgrTest, parseCatchmentArea) {
    // Ungültige Zeichen in erster Zeile
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   fooo\n"
        "   10001010\n"
        "   "
    ), std::runtime_error);

    // Erste Zeile muss leer sein
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   00111100\n"
        "   11111111\n"
        "   00111100\n"
        "   "
    ), std::runtime_error);

    // ungültiges Zeichen
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   \n"
        "   0011XX00\n"
        "   00111100\n"
        "   "
    ), std::runtime_error);

    // Letzte Zeile nicht vorhanden
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   \n"
        "   00110000\n"
        "   00111100"
    ), std::runtime_error);

    // Zwar ok, aber Breite nicht einheitlich
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "     \n"
        "   0011110\n"
        "   11111111\n"
        "   000000001\n"
        " "
    ), std::runtime_error);

    // Korrekte Werte
    RectangleData<char>* catchmentArea = ConfigMgr::parseCatchmentArea(
        "     \n"
        "   00111100\n"
        "   11111111\n"
        "   00000000\n"
        " "
    );
    ASSERT_EQ(8, catchmentArea->width);
    ASSERT_EQ(3, catchmentArea->height);
    ASSERT_EQ(0, memcmp("001111001111111100000000", catchmentArea->data, 24));
    delete catchmentArea;

    catchmentArea = ConfigMgr::parseCatchmentArea(
        "     \n"
        "      01001\n"
        "   10001\n"
        "     01001\n"
        "           01001\n"
        ""
    );
    ASSERT_EQ(5, catchmentArea->width);
    ASSERT_EQ(4, catchmentArea->height);
    ASSERT_EQ(0, memcmp("01001100010100101001", catchmentArea->data, 20));
    delete catchmentArea;
}