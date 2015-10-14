#include "GameTest.h"


class InCatchmentAreaFinderInvisibleGoodsTest : public GameTest {

protected:
    Building* sheepFarm;
    const Good* goodGrassland;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        sheepFarm = dynamic_cast<Building*>(game->addStructure(
            MapCoords(30, 30), configMgr->getMapObjectType("sheep-farm"),
            Direction::SOUTH, game->getPlayer(0)));

        goodGrassland = context.configMgr->getGood("grassland");
    }
};


/**
 * @brief Testet mit InCatchmentAreaFinder, ob direkt nach Spielstart Grassland verfügbar ist.
 */
TEST_F(InCatchmentAreaFinderInvisibleGoodsTest, checkThatAfterGameStartGrasslandCanBeHarvested) {
    CatchmentAreaIterator catchmentAreaIterator(*sheepFarm, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);

    InCatchmentAreaFinderResult result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(goodGrassland);

    ASSERT_EQ(true, result.foundSomething);
}


/**
 * @brief Testet InCatchmentAreaFinder, ob die Schaffarm überhaupt eine Route für ein Schaf findet.
 * Wir grasen hierzu alle Felder bis auf eins ab.
 */
TEST_F(InCatchmentAreaFinderInvisibleGoodsTest, checkThatRouteToGrasslandIsFound) {
    // Testaufbau
    for (int mapY = 20; mapY <= 40; mapY++) {
        for (int mapX = 20; mapX <= 40; mapX++) {
            game->getMap()->getMapTileAt({mapX, mapY})->nextHarvestTicks = 5000;
        }
    }
    game->getMap()->getMapTileAt({30, 34})->nextHarvestTicks = 0;

    // Testdurchführung
    CatchmentAreaIterator catchmentAreaIterator(*sheepFarm, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);

    InCatchmentAreaFinderResult result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(goodGrassland);

    ASSERT_EQ(true, result.foundSomething);
    ASSERT_EQ(MapCoords(30, 34), result.mapCoords);
    ASSERT_EQ(MapCoords(30, 34), *result.route.crbegin());
}


/**
 * @brief Testet InCatchmentAreaFinder, ob keine Route gefunden wird, wenn alle Felder abgegrast sind.
 */
TEST_F(InCatchmentAreaFinderInvisibleGoodsTest, checkThatNoRouteIsFoundWhenAllWasHarvested) {
    // Testaufbau
    for (int mapY = 20; mapY <= 40; mapY++) {
        for (int mapX = 20; mapX <= 40; mapX++) {
            game->getMap()->getMapTileAt({mapX, mapY})->nextHarvestTicks = 50000;
        }
    }

    // Testdurchführung
    CatchmentAreaIterator catchmentAreaIterator(*sheepFarm, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);

    InCatchmentAreaFinderResult result = inCatchmentAreaFinder.findMapTileWithInvisibleGood(goodGrassland);

    ASSERT_EQ(false, result.foundSomething);
}