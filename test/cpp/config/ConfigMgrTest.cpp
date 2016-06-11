#include <algorithm>
#include <cstring>
#include <list>
#include <gtest/gtest.h>
#include "config/ConfigMgr.h"
#include "config/ErrorInConfigException.h"
#include "config/MapTileType.h"
#include "map/MapObjectType.h"


/**
 * @brief Test, ob die Konfiguration der Map-Objekt-Typen korrekt geladen wird.
 */
TEST(ConfigMgrTest, loadMapObjectTypes) {
    ConfigMgr configMgr("data/config");

    // Teste 2 Objekte komplett

    const MapObjectType* cathedral = configMgr.getMapObjectType("cathedral");
    ASSERT_TRUE(cathedral != nullptr);
    ASSERT_EQ(MapObjectTypeClass::BUILDING, cathedral->type);
    ASSERT_EQ("cathedral", cathedral->name);
    ASSERT_EQ("mapObjectType|cathedral", cathedral->getTitleMsgid());
    ASSERT_EQ(6, cathedral->mapWidth);
    ASSERT_EQ(4, cathedral->mapHeight);
    ASSERT_EQ("mapobjects/cathedral", cathedral->graphicSetName);
    ASSERT_EQ(StructurePlacing::INDIVIDUALLY, cathedral->structurePlacing);
    ASSERT_EQ((MapTileTypeInt) MapTileType::GRASS, cathedral->placeableOnMapTileTypeMask);
    ASSERT_EQ(50, cathedral->catchmentArea->width);
    ASSERT_EQ(50, cathedral->catchmentArea->height);
    ASSERT_EQ(0, memcmp("0000000000000000000011111111110000000000000000000000000000000000001111111111111111110000000000000000000000000000001111111111111111111111000000000000000000000000001111111111111111111111111100000000000000000000000111111111111111111111111111100000000000000000000111111111111111111111111111111110000000000000000011111111111111111111111111111111110000000000000001111111111111111111111111111111111110000000000000111111111111111111111111111111111111110000000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000011111111111111111111111111111111111111111100000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111111000011111111111111111111111111111111111111111111110001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111001111111111111111111111111111111111111111111111110011111111111111111111111111111111111111111111111100111111111111111111111111111111111111111111111111000111111111111111111111111111111111111111111111100001111111111111111111111111111111111111111111111000001111111111111111111111111111111111111111111100000011111111111111111111111111111111111111111111000000011111111111111111111111111111111111111111100000000011111111111111111111111111111111111111110000000000111111111111111111111111111111111111111100000000000111111111111111111111111111111111111110000000000000111111111111111111111111111111111111000000000000000111111111111111111111111111111111100000000000000000111111111111111111111111111111110000000000000000000011111111111111111111111111110000000000000000000000011111111111111111111111111000000000000000000000000001111111111111111111111000000000000000000000000000000111111111111111111000000000000000000000000000000000000111111111100000000000000000000",
                        cathedral->catchmentArea->data, 2500));
    ASSERT_EQ(false, cathedral->increasesBuildableArea);
    ASSERT_EQ(7500, cathedral->buildingCosts.coins);
    ASSERT_EQ(25, cathedral->buildingCosts.tools);
    ASSERT_EQ(25, cathedral->buildingCosts.wood);
    ASSERT_EQ(70, cathedral->buildingCosts.bricks);
    ASSERT_EQ(90, cathedral->operatingCosts.running);
    ASSERT_EQ(cathedral->operatingCosts.running, cathedral->operatingCosts.decommissioned);
    ASSERT_EQ(false, cathedral->buildingProduction.output.isUsed());
    ASSERT_EQ(false, cathedral->buildingProduction.input.isUsed());
    ASSERT_EQ(false, cathedral->buildingProduction.input2.isUsed());
    ASSERT_EQ(0, cathedral->goodsCapacity);
    ASSERT_EQ(5, cathedral->inhabitants);
    ASSERT_EQ(nullptr, cathedral->populationTier);
    ASSERT_TRUE(cathedral->carrier.mapObjectType == nullptr);
    ASSERT_EQ(0, cathedral->carrier.capacity);
    ASSERT_TRUE(cathedral->carrier.animations.empty());
    ASSERT_EQ(0, cathedral->secondsToProduce);
    ASSERT_EQ(0, cathedral->inputAmountForProduction);
    ASSERT_EQ(0, cathedral->input2AmountForProduction);
    ASSERT_EQ(0, cathedral->maxAge);
    ASSERT_EQ(false, cathedral->isForest);

    const MapObjectType* sugarcaneField = configMgr.getMapObjectType("sugarcane-field");
    ASSERT_TRUE(sugarcaneField != nullptr);
    ASSERT_EQ(MapObjectTypeClass::HARVESTABLE, sugarcaneField->type);
    ASSERT_EQ("sugarcane-field", sugarcaneField->name);
    ASSERT_EQ("mapObjectType|sugarcane-field", sugarcaneField->getTitleMsgid());
    ASSERT_EQ(1, sugarcaneField->mapWidth);
    ASSERT_EQ(1, sugarcaneField->mapHeight);
    ASSERT_EQ("mapobjects/sugarcane-field", sugarcaneField->graphicSetName);
    ASSERT_EQ(StructurePlacing::RECTANGLE, sugarcaneField->structurePlacing);
    ASSERT_EQ((MapTileTypeInt) MapTileType::GRASS, sugarcaneField->placeableOnMapTileTypeMask);
    ASSERT_TRUE(sugarcaneField->catchmentArea == nullptr);
    ASSERT_EQ(false, sugarcaneField->increasesBuildableArea);
    ASSERT_EQ(5, sugarcaneField->buildingCosts.coins);
    ASSERT_EQ(0, sugarcaneField->buildingCosts.tools);
    ASSERT_EQ(0, sugarcaneField->buildingCosts.wood);
    ASSERT_EQ(0, sugarcaneField->buildingCosts.bricks);
    ASSERT_EQ(0, sugarcaneField->operatingCosts.running);
    ASSERT_EQ(sugarcaneField->operatingCosts.running, sugarcaneField->operatingCosts.decommissioned);
    ASSERT_EQ(false, sugarcaneField->buildingProduction.output.isUsed());
    ASSERT_EQ(false, sugarcaneField->buildingProduction.input.isUsed());
    ASSERT_EQ(false, sugarcaneField->buildingProduction.input2.isUsed());
    ASSERT_EQ(0, sugarcaneField->goodsCapacity);
    ASSERT_EQ(0, sugarcaneField->inhabitants);
    ASSERT_EQ(nullptr, sugarcaneField->populationTier);
    ASSERT_TRUE(sugarcaneField->carrier.mapObjectType == nullptr);
    ASSERT_EQ(0, sugarcaneField->carrier.capacity);
    ASSERT_TRUE(sugarcaneField->carrier.animations.empty());
    ASSERT_EQ(0, sugarcaneField->secondsToProduce);
    ASSERT_EQ(0, sugarcaneField->inputAmountForProduction);
    ASSERT_EQ(0, sugarcaneField->input2AmountForProduction);
    ASSERT_EQ(4, sugarcaneField->maxAge);
    ASSERT_EQ(false, sugarcaneField->isForest);

    // Teste sonstige Property-Inhalte, die obig nicht abgedeckt wurden

    const MapObjectType* square1 = configMgr.getMapObjectType("square1");
    ASSERT_EQ(MapObjectTypeClass::STRUCTURE, square1->type);

    const MapObjectType* cobbledStreet = configMgr.getMapObjectType("cobbled-street");
    ASSERT_EQ(MapObjectTypeClass::STREET, cobbledStreet->type);
    ASSERT_EQ(StructurePlacing::PATH, cobbledStreet->structurePlacing);

    const MapObjectType* toolsmiths = configMgr.getMapObjectType("toolsmiths");
    ASSERT_EQ(150, toolsmiths->buildingCosts.coins);
    ASSERT_EQ(3, toolsmiths->buildingCosts.tools);
    ASSERT_EQ(2, toolsmiths->buildingCosts.wood);
    ASSERT_EQ(5, toolsmiths->buildingCosts.bricks);
    ASSERT_EQ(25, toolsmiths->operatingCosts.running);
    ASSERT_EQ(10, toolsmiths->operatingCosts.decommissioned);
    ASSERT_TRUE(toolsmiths->carrier.mapObjectType == nullptr);
    ASSERT_EQ(0, toolsmiths->carrier.capacity);
    ASSERT_TRUE(toolsmiths->carrier.animations.empty());

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
    ASSERT_EQ(23, distillery->secondsToProduce);
    ASSERT_EQ("sugar", distillery->buildingProduction.input.good->name);
    ASSERT_EQ(5, distillery->buildingProduction.input.capacity);
    ASSERT_EQ(2.0, distillery->inputAmountForProduction);
    // TODO Testcase für output, input+inputAmountForProduction UND input2+input2AmountForProduction gesetzt;
    // aktuell haben wir kein solches Gebäude

    const MapObjectType* office1 = configMgr.getMapObjectType("office1");
    ASSERT_EQ(true, office1->increasesBuildableArea);
    ASSERT_EQ(30, office1->goodsCapacity);

    const MapObjectType* pier = configMgr.getMapObjectType("pier");
    ASSERT_EQ(
        (MapTileTypeInt) MapTileType::SHORE_OCEAN | (MapTileTypeInt) MapTileType::SHORE_GRASS,
        pier->placeableOnMapTileTypeMask);

    const MapObjectType* northernForest1 = configMgr.getMapObjectType("northern-forest1");
    ASSERT_EQ(true, northernForest1->isForest);

    const MapObjectType* sheepFarm = configMgr.getMapObjectType("sheep-farm");
    ASSERT_EQ(30, sheepFarm->secondsToProduce);
    ASSERT_EQ(4, sheepFarm->inputAmountForProduction);

    const MapObjectType* settlersHouse3 = configMgr.getMapObjectType("settlers-house3");
    const PopulationTier* settlers = configMgr.getPopulationTier("settlers");
    ASSERT_EQ(settlers, settlersHouse3->populationTier);

    const MapObjectType* grainMill = configMgr.getMapObjectType("grain-mill");
    ASSERT_TRUE(grainMill->animateWhenWorking);
}

/**
 * @brief Test, ob die Konfiguration der Map-Objekt-Typen (speziell Träger) korrekt geladen wird.
 */
TEST(ConfigMgrTest, loadCarrierMapObjectTypes) {
    ConfigMgr configMgr("data/config");

    const MapObjectType* carrier = configMgr.getMapObjectType("carrier");
    ASSERT_TRUE(carrier != nullptr);
    ASSERT_EQ(MapObjectTypeClass::CARRIER, carrier->type);
    ASSERT_EQ(4, carrier->carrier.capacity);
    ASSERT_EQ(0, carrier->secondsToProduce);

    const MapObjectType* cart = configMgr.getMapObjectType("cart");
    ASSERT_TRUE(cart != nullptr);
    ASSERT_EQ(MapObjectTypeClass::CARRIER, cart->type);
    ASSERT_EQ(6, cart->carrier.capacity);
    ASSERT_EQ(0, cart->secondsToProduce);

    const MapObjectType* sheep = configMgr.getMapObjectType("sheep");
    ASSERT_TRUE(sheep != nullptr);
    ASSERT_EQ(MapObjectTypeClass::CARRIER, sheep->type);
    ASSERT_EQ(4, sheep->carrier.capacity);
    ASSERT_EQ(18, sheep->secondsToProduce);
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
    ), ErrorInConfigException);

    // Erste Zeile muss leer sein
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   00111100\n"
        "   11111111\n"
        "   00111100\n"
        "   "
    ), ErrorInConfigException);

    // ungültiges Zeichen
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   \n"
        "   0011XX00\n"
        "   00111100\n"
        "   "
    ), ErrorInConfigException);

    // Letzte Zeile nicht vorhanden
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "   \n"
        "   00110000\n"
        "   00111100"
    ), ErrorInConfigException);

    // Zwar ok, aber Breite nicht einheitlich
    ASSERT_THROW(ConfigMgr::parseCatchmentArea(
        "     \n"
        "   0011110\n"
        "   11111111\n"
        "   000000001\n"
        " "
    ), ErrorInConfigException);

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

/**
 * @brief Testet, ob das Parsen des Einzugsbereichs aus der XML mit unterschiedlichen Newline-Endings funktioniert
 */
TEST(ConfigMgrTest, parseCatchmentAreaDifferentLineEndings) {
    // Linux
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

    // Windows
    catchmentArea = ConfigMgr::parseCatchmentArea(
        "     \r\n"
        "   00111100\r\n"
        "   11111111\r\n"
        "   00000000\r\n"
        " "
    );
    ASSERT_EQ(8, catchmentArea->width);
    ASSERT_EQ(3, catchmentArea->height);
    ASSERT_EQ(0, memcmp("001111001111111100000000", catchmentArea->data, 24));
    delete catchmentArea;

    // Mac
    catchmentArea = ConfigMgr::parseCatchmentArea(
        "     \r"
        "   00111100\r"
        "   11111111\r"
        "   00000000\r"
        " "
    );
    ASSERT_EQ(8, catchmentArea->width);
    ASSERT_EQ(3, catchmentArea->height);
    ASSERT_EQ(0, memcmp("001111001111111100000000", catchmentArea->data, 24));
    delete catchmentArea;
}

/**
 * @brief Prüft, ob alle Bevölkerungsgruppen richtig konfiguriert sind.
 */
TEST(ConfigMgrTest, checkThatPopulationTiersAreFilledCorrectly) {
    ConfigMgr configMgr("data/config");

    {
        const PopulationTier* populationTier = configMgr.getPopulationTier("pioneers");
        ASSERT_EQ(1, populationTier->index);
        ASSERT_EQ("pioneers", populationTier->name);
        ASSERT_EQ("populationTier|pioneers|singular", populationTier->getTitleMsgid(false));
        ASSERT_EQ("populationTier|pioneers|plural", populationTier->getTitleMsgid(true));

        ASSERT_EQ(0, populationTier->advancementMissingGoodsOk);
        ASSERT_EQ(0, populationTier->advancementCosts.coins);
        ASSERT_EQ(0, populationTier->advancementCosts.tools);
        ASSERT_EQ(3, populationTier->advancementCosts.wood);
        ASSERT_EQ(0, populationTier->advancementCosts.bricks);

        ASSERT_TRUE(populationTier->needsGoods.empty());
        ASSERT_TRUE(populationTier->needsPublicBuildings.empty());

        ASSERT_EQ(2, populationTier->maxPopulationPerHouse);
        ASSERT_EQ(1.4, populationTier->taxesPerInhabitant);
    }

    {
        const PopulationTier* populationTier = configMgr.getPopulationTier("settlers");
        ASSERT_EQ(2, populationTier->index);
        ASSERT_EQ("settlers", populationTier->name);
        ASSERT_EQ("populationTier|settlers|singular", populationTier->getTitleMsgid(false));
        ASSERT_EQ("populationTier|settlers|plural", populationTier->getTitleMsgid(true));

        ASSERT_EQ(1, populationTier->advancementMissingGoodsOk);
        ASSERT_EQ(0, populationTier->advancementCosts.coins);
        ASSERT_EQ(1, populationTier->advancementCosts.tools);
        ASSERT_EQ(3, populationTier->advancementCosts.wood);
        ASSERT_EQ(0, populationTier->advancementCosts.bricks);

        {
            auto iter = populationTier->needsGoods.cbegin();
            ASSERT_EQ("cloth", iter->good->name);
            ASSERT_EQ(0.6, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("alcohol", iter->good->name);
            ASSERT_EQ(0.5, iter->consumePerCycle);
            iter++;
            ASSERT_EQ(populationTier->needsGoods.cend(), iter);
        }

        {
            auto iter = populationTier->needsPublicBuildings.cbegin();
            ASSERT_EQ("marketplace", (*iter)->name);
            iter++;
            ASSERT_EQ("chapel", (*iter)->name);
            iter++;
            ASSERT_EQ(populationTier->needsPublicBuildings.cend(), iter);
        }

        ASSERT_EQ(6, populationTier->maxPopulationPerHouse);
        ASSERT_EQ(1.6, populationTier->taxesPerInhabitant);
    }

    {
        const PopulationTier* populationTier = configMgr.getPopulationTier("burghers");
        ASSERT_EQ(3, populationTier->index);
        ASSERT_EQ("burghers", populationTier->name);
        ASSERT_EQ("populationTier|burghers|singular", populationTier->getTitleMsgid(false));
        ASSERT_EQ("populationTier|burghers|plural", populationTier->getTitleMsgid(true));

        ASSERT_EQ(1, populationTier->advancementMissingGoodsOk);
        ASSERT_EQ(0, populationTier->advancementCosts.coins);
        ASSERT_EQ(2, populationTier->advancementCosts.tools);
        ASSERT_EQ(2, populationTier->advancementCosts.wood);
        ASSERT_EQ(6, populationTier->advancementCosts.bricks);

        {
            auto iter = populationTier->needsGoods.cbegin();
            ASSERT_EQ("cloth", iter->good->name);
            ASSERT_EQ(0.7, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("alcohol", iter->good->name);
            ASSERT_EQ(0.6, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("tobacco-goods", iter->good->name);
            ASSERT_EQ(0.6, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("spices", iter->good->name);
            ASSERT_EQ(0.6, iter->consumePerCycle);
            iter++;
            ASSERT_EQ(populationTier->needsGoods.cend(), iter);
        }

        {
            auto iter = populationTier->needsPublicBuildings.cbegin();
            ASSERT_EQ("marketplace", (*iter)->name);
            iter++;
            ASSERT_EQ("chapel", (*iter)->name);
            iter++;
            ASSERT_EQ("tavern", (*iter)->name);
            iter++;
            ASSERT_EQ("school", (*iter)->name);
            iter++;
            ASSERT_EQ(populationTier->needsPublicBuildings.cend(), iter);
        }

        ASSERT_EQ(15, populationTier->maxPopulationPerHouse);
        ASSERT_EQ(2.1, populationTier->taxesPerInhabitant);
    }

    {
        const PopulationTier* populationTier = configMgr.getPopulationTier("merchants");
        ASSERT_EQ(4, populationTier->index);
        ASSERT_EQ("merchants", populationTier->name);
        ASSERT_EQ("populationTier|merchants|singular", populationTier->getTitleMsgid(false));
        ASSERT_EQ("populationTier|merchants|plural", populationTier->getTitleMsgid(true));

        ASSERT_EQ(1, populationTier->advancementMissingGoodsOk);
        ASSERT_EQ(0, populationTier->advancementCosts.coins);
        ASSERT_EQ(3, populationTier->advancementCosts.tools);
        ASSERT_EQ(3, populationTier->advancementCosts.wood);
        ASSERT_EQ(9, populationTier->advancementCosts.bricks);

        {
            auto iter = populationTier->needsGoods.cbegin();
            ASSERT_EQ("cloth", iter->good->name);
            ASSERT_EQ(0.8, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("alcohol", iter->good->name);
            ASSERT_EQ(0.7, iter->consumePerCycle);
            iter++;
//            ASSERT_EQ("cocoa", iter->good->name);
//            ASSERT_EQ(0.7, iter->consumePerCycle);
//            iter++;
            ASSERT_EQ("tobacco-goods", iter->good->name);
            ASSERT_EQ(0.6, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("spices", iter->good->name);
            ASSERT_EQ(0.6, iter->consumePerCycle);
            iter++;
            ASSERT_EQ(populationTier->needsGoods.cend(), iter);
        }

        {
            auto iter = populationTier->needsPublicBuildings.cbegin();
            ASSERT_EQ("marketplace", (*iter)->name);
            iter++;
            ASSERT_EQ("tavern", (*iter)->name);
            iter++;
            ASSERT_EQ("school", (*iter)->name);
            iter++;
//            ASSERT_EQ("church", (*iter)->name);
//            iter++;
//            ASSERT_EQ("bathhouse", (*iter)->name);
//            iter++;
            ASSERT_EQ(populationTier->needsPublicBuildings.cend(), iter);
        }

        ASSERT_EQ(25, populationTier->maxPopulationPerHouse);
        ASSERT_EQ(2.4, populationTier->taxesPerInhabitant);
    }

    {
        const PopulationTier* populationTier = configMgr.getPopulationTier("aristocrats");
        ASSERT_EQ(5, populationTier->index);
        ASSERT_EQ("aristocrats", populationTier->name);
        ASSERT_EQ("populationTier|aristocrats|singular", populationTier->getTitleMsgid(false));
        ASSERT_EQ("populationTier|aristocrats|plural", populationTier->getTitleMsgid(true));

        ASSERT_EQ(0, populationTier->advancementMissingGoodsOk);
        ASSERT_EQ(0, populationTier->advancementCosts.coins);
        ASSERT_EQ(3, populationTier->advancementCosts.tools);
        ASSERT_EQ(3, populationTier->advancementCosts.wood);
        ASSERT_EQ(12, populationTier->advancementCosts.bricks);

        {
            auto iter = populationTier->needsGoods.cbegin();
            ASSERT_EQ("alcohol", iter->good->name);
            ASSERT_EQ(0.8, iter->consumePerCycle);
            iter++;
//            ASSERT_EQ("cocoa", iter->good->name);
//            ASSERT_EQ(0.6, iter->consumePerCycle);
//            iter++;
            ASSERT_EQ("tobacco-goods", iter->good->name);
            ASSERT_EQ(0.5, iter->consumePerCycle);
            iter++;
            ASSERT_EQ("spices", iter->good->name);
            ASSERT_EQ(0.5, iter->consumePerCycle);
            iter++;
//            ASSERT_EQ("clothing", iter->good->name);
//            ASSERT_EQ(0.5, iter->consumePerCycle);
//            iter++;
//            ASSERT_EQ("jewelry", iter->good->name);
//            ASSERT_EQ(0.2, iter->consumePerCycle);
//            iter++;
            ASSERT_EQ(populationTier->needsGoods.cend(), iter);
        }

        {
            auto iter = populationTier->needsPublicBuildings.cbegin();
            ASSERT_EQ("marketplace", (*iter)->name);
            iter++;
            ASSERT_EQ("tavern", (*iter)->name);
            iter++;
//            ASSERT_EQ("church", (*iter)->name);
//            iter++;
//            ASSERT_EQ("bathhouse", (*iter)->name);
//            iter++;
//            ASSERT_EQ("high-school", (*iter)->name);
//            iter++;
//            ASSERT_EQ("theater", (*iter)->name);
//            iter++;
            ASSERT_EQ(populationTier->needsPublicBuildings.cend(), iter);
        }

        ASSERT_EQ(40, populationTier->maxPopulationPerHouse);
        ASSERT_EQ(2.6, populationTier->taxesPerInhabitant);
    }
}

/**
 * @brief Prüft, ob die Nahrungsbedürfnisse korrekt geladen wurden.
 */
TEST(ConfigMgrTest, checkThatFoodNeedsIsFilledCorrectly) {
    ConfigMgr configMgr("data/config");

    const Good* food = configMgr.getGood("food");
    ASSERT_TRUE(food != nullptr);

    const NeededGood& foodGood = configMgr.getFoodGood();
    ASSERT_EQ(food, foodGood.good);
    ASSERT_EQ(1.3, foodGood.consumePerCycle);
}

/**
 * @brief Prüft, ob alle Bevölkerungsgruppen im Set richtig sortiert sind
 */
TEST(ConfigMgrTest, checkThatPopulationTiersAreSortedInSet) {
    ConfigMgr configMgr("data/config");
    const std::set<PopulationTier>& allPopulationTiers = configMgr.getAllPopulationTiers();

    // Größe des Sets überprüfen
    ASSERT_EQ(5, allPopulationTiers.size());

    // Reihenfolge überprüfen
    auto iter = allPopulationTiers.cbegin();

    ASSERT_EQ("pioneers", iter->name);
    iter++;

    ASSERT_EQ("settlers", iter->name);
    iter++;

    ASSERT_EQ("burghers", iter->name);
    iter++;

    ASSERT_EQ("merchants", iter->name);
    iter++;

    ASSERT_EQ("aristocrats", iter->name);
    iter++;

    ASSERT_TRUE(iter == allPopulationTiers.cend());
}

/**
 * @brief Prüft, ob eine Exception fliegt, wenn ein ungültiger Dateipfad angegeben wird
 */
TEST(ConfigMgrTest, checkForExceptionWhenFileNotExists) {
    ASSERT_ANY_THROW(ConfigMgr("this/path/does/not/exist"));
}

/**
 * @brief Prüft, ob eine Exception fliegt, wenn Parse-Fehler in einer Datei sind.
 */
TEST(ConfigMgrTest, checkForExceptionWhenThereAreParseErrors) {
    // population-tiers.xml ist kaputt und muss zur Exception führen
    ASSERT_THROW(ConfigMgr("data/config-with-parse-errors"), ErrorInConfigException);
}

TEST(ConfigMgrTest, checkGetMapObjectTypesByPopulationTier) {
    ConfigMgr configMgr("data/config");

    const PopulationTier* settlers = configMgr.getPopulationTier("settlers");
    std::list<const MapObjectType*> mapObjectTypes = configMgr.getMapObjectTypesByPopulationTier(settlers);
    ASSERT_EQ(5, mapObjectTypes.size());
    ASSERT_TRUE(
        std::find_if(mapObjectTypes.cbegin(), mapObjectTypes.cend(),
                     [](const MapObjectType* mapObjectType) { return mapObjectType->name == "settlers-house1"; }
        ) != mapObjectTypes.cend()
    );
    ASSERT_TRUE(
        std::find_if(mapObjectTypes.cbegin(), mapObjectTypes.cend(),
                     [](const MapObjectType* mapObjectType) { return mapObjectType->name == "settlers-house2"; }
        ) != mapObjectTypes.cend()
    );
    ASSERT_TRUE(
        std::find_if(mapObjectTypes.cbegin(), mapObjectTypes.cend(),
                     [](const MapObjectType* mapObjectType) { return mapObjectType->name == "settlers-house3"; }
        ) != mapObjectTypes.cend()
    );
    ASSERT_TRUE(
        std::find_if(mapObjectTypes.cbegin(), mapObjectTypes.cend(),
                     [](const MapObjectType* mapObjectType) { return mapObjectType->name == "settlers-house4"; }
        ) != mapObjectTypes.cend()
    );
    ASSERT_TRUE(
        std::find_if(mapObjectTypes.cbegin(), mapObjectTypes.cend(),
                     [](const MapObjectType* mapObjectType) { return mapObjectType->name == "settlers-house5"; }
        ) != mapObjectTypes.cend()
    );

    const PopulationTier* pioneers = configMgr.getPopulationTier("pioneers");
    ASSERT_EQ(5, configMgr.getMapObjectTypesByPopulationTier(pioneers).size());
}

/**
 * @brief Test, ob die Konfiguration der Schiffe korrekt geladen wird.
 */
TEST(ConfigMgrTest, loadShipMapObjectTypes) {
    ConfigMgr configMgr("data/config");

    const MapObjectType* littleShip = configMgr.getMapObjectType("little-ship");
    ASSERT_TRUE(littleShip != nullptr);
    ASSERT_EQ(MapObjectTypeClass::SHIP, littleShip->type);
    ASSERT_EQ(2, littleShip->mapWidth);
    ASSERT_EQ(2, littleShip->mapHeight);
    ASSERT_EQ(4, littleShip->goodsSlots);
}