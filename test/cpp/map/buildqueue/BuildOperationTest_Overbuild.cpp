#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"
#include "map/Street.h"


/**
 * @brief Testet das Überbauen, wenn was im Weg is.
 *
 * Wir versuchen einen Förster über Straße und Haus zu bauen auf (47, 43).
 */
TEST_F(BuildOperationTest, overbuildBuildSomethingInTheWay) {
    const MapObjectType* foresters = configMgr->getMapObjectType("foresters");

    BuildOperation buildOperation(context, *player);
    buildOperation.requestBuildWhenNothingInTheWay({47, 43}, foresters, Direction::EAST);
    const BuildOperationResult& result = buildOperation.getResult();

    // erwarte, dass der Request abgelehnt wurde.
    ASSERT_EQ(true, result.empty());
}

/**
 * @brief Testet das komplette Überbauen aller Felder.
 *
 * Wir bauen einen Förster vollständig über alle 1x1 Felder auf (43, 39)
 */
TEST_F(BuildOperationTest, overbuildBuildCompletely) {
    const MapObjectType* foresters = configMgr->getMapObjectType("foresters");

    player->coins = 1000;
    colony->getGoods(configMgr->getGood("tools")).inventory = 5;

    BuildOperation buildOperation(context, *player);
    buildOperation.requestBuildWhenNothingInTheWay({43, 39}, foresters, Direction::SOUTH);
    const BuildOperationResult& result = buildOperation.getResult();

    // Result auswerten

    ASSERT_EQ(4, result.size());
    ASSERT_EQ(4, result.at({43, 39}).use_count()); // Erwarte, dass alle 4 Kacheln dasselbe ResultBit haben

    const BuildOperationResultBit& resultBit = *result.at({43, 39});
    ASSERT_EQ(foresters, resultBit.mapObjectToReplaceWith->getMapObjectType());
    ASSERT_EQ(resultBit.mapObjectToReplaceWith, resultBit.mapObjectToDraw);

    // Jetzt wirklich bauen

    unsigned long mapObjectsCountBefore = game->getMap()->getMapObjects().size();
    buildOperation.doBuild();
    unsigned long mapObjectsCountAfter = game->getMap()->getMapObjects().size();

    ASSERT_EQ(950, player->coins);
    ASSERT_EQ(3, colony->getGoods(configMgr->getGood("tools")).inventory);

    // Sicherstellen, dass die Anzahl der Map-Objekte stimmt. Es müssen 4 gelöscht und 1 neues dazugekommen sein
    ASSERT_EQ(mapObjectsCountBefore - 3, mapObjectsCountAfter);

    assertCorrectBuild({43, 39}, foresters, Direction::SOUTH, 2, 2);
    ASSERT_EQ(foresters, game->getMap()->getMapObjectFixedAt({44, 40})->getMapObjectType());
}

/**
 * @brief Testet das partielle Überbauen.
 *
 * Wir bauen einen Förster auf (42, 42) und überbauen nur 2 von 4 Feldern.
 */
TEST_F(BuildOperationTest, overbuildBuildPartially) {
    const MapObjectType* foresters = configMgr->getMapObjectType("foresters");

    player->coins = 1000;
    colony->getGoods(configMgr->getGood("tools")).inventory = 5;

    BuildOperation buildOperation(context, *player);
    buildOperation.requestBuildWhenNothingInTheWay({42, 42}, foresters, Direction::SOUTH);
    const BuildOperationResult& result = buildOperation.getResult();

    // Result auswerten

    ASSERT_EQ(4, result.size());
    ASSERT_EQ(4, result.at({42, 42}).use_count()); // Erwarte, dass alle 4 Kacheln dasselbe ResultBit haben

    const BuildOperationResultBit& resultBit = *result.at({42, 42});
    ASSERT_EQ(foresters, resultBit.mapObjectToReplaceWith->getMapObjectType());
    ASSERT_EQ(resultBit.mapObjectToReplaceWith, resultBit.mapObjectToDraw);

    // Jetzt wirklich bauen

    unsigned long mapObjectsCountBefore = game->getMap()->getMapObjects().size();
    buildOperation.doBuild();
    unsigned long mapObjectsCountAfter = game->getMap()->getMapObjects().size();

    ASSERT_EQ(950, player->coins);
    ASSERT_EQ(3, colony->getGoods(configMgr->getGood("tools")).inventory);

    // Sicherstellen, dass die Anzahl der Map-Objekte stimmt. Es müssen 2 gelöscht und 1 neues dazugekommen sein
    ASSERT_EQ(mapObjectsCountBefore - 1, mapObjectsCountAfter);

    assertCorrectBuild({42, 42}, foresters, Direction::SOUTH, 2, 2);
    ASSERT_EQ(foresters, game->getMap()->getMapObjectFixedAt({43, 42})->getMapObjectType());
    ASSERT_EQ(foresters, game->getMap()->getMapObjectFixedAt({43, 43})->getMapObjectType());
}


/**
 * @brief Überprüft verschiedene Kombinationen mit Überbauen von Straßen
 *
 * - Feldweg über Straße (muss gehen)
 * - Straße über Feldweg (muss gehen)
 * - Straße über Straße (muss ignoriert werden)
 * - Straße über Platz (muss ignoriert werden)
 * - Platz über Feldweg (muss gehen)
 * - Tabakfeld über Wald (muss gehen)
 * - Gewürzfeld über Gewürzfeld (muss ignoriert werden)
 */
TEST_F(BuildOperationTest, overbuildBuildStreetOverStreets) {
    const MapObjectType* farmRoad = configMgr->getMapObjectType("farm-road");
    const MapObjectType* cobbledStreet = configMgr->getMapObjectType("cobbled-street");
    const MapObjectType* square1 = configMgr->getMapObjectType("square1");
    const MapObjectType* northernForest1 = configMgr->getMapObjectType("northern-forest1");
    const MapObjectType* tobaccoField = configMgr->getMapObjectType("tobacco-field");
    const MapObjectType* spicesField = configMgr->getMapObjectType("spices-field");

    player->coins = 1000;
    colony->getGoods(configMgr->getGood("bricks")).inventory = 10;

    // Feldweg über Straße
    {
        ASSERT_EQ(cobbledStreet, game->getMap()->getMapObjectFixedAt({44, 36})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({44, 36}, farmRoad, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(farmRoad, game->getMap()->getMapObjectFixedAt({44, 36})->getMapObjectType());
        ASSERT_EQ(995, player->coins);
        ASSERT_EQ(10, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }

    // Straße über Feldweg
    {
        ASSERT_EQ(farmRoad, game->getMap()->getMapObjectFixedAt({46, 32})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({46, 32}, cobbledStreet, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(cobbledStreet, game->getMap()->getMapObjectFixedAt({46, 32})->getMapObjectType());
        ASSERT_EQ(990, player->coins);
        ASSERT_EQ(9, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }

    // Straße über Straße
    {
        ASSERT_EQ(cobbledStreet, game->getMap()->getMapObjectFixedAt({44, 35})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({44, 35}, cobbledStreet, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(cobbledStreet, game->getMap()->getMapObjectFixedAt({44, 35})->getMapObjectType());
        ASSERT_EQ(990, player->coins);
        ASSERT_EQ(9, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }

    // Straße über Platz
    {
        ASSERT_EQ(square1, game->getMap()->getMapObjectFixedAt({41, 35})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({41, 35}, cobbledStreet, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(square1, game->getMap()->getMapObjectFixedAt({41, 35})->getMapObjectType());
        ASSERT_EQ(990, player->coins);
        ASSERT_EQ(9, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }

    // Platz über Feldweg
    {
        ASSERT_EQ(farmRoad, game->getMap()->getMapObjectFixedAt({47, 32})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({47, 32}, square1, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(square1, game->getMap()->getMapObjectFixedAt({47, 32})->getMapObjectType());
        ASSERT_EQ(985, player->coins);
        ASSERT_EQ(8, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }

    // Tabakfeld über Wald
    {
        ASSERT_EQ(northernForest1, game->getMap()->getMapObjectFixedAt({45, 42})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({45, 42}, tobaccoField, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(tobaccoField, game->getMap()->getMapObjectFixedAt({45, 42})->getMapObjectType());
        ASSERT_EQ(980, player->coins);
        ASSERT_EQ(8, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }

    // Gewürzfeld über Gewürzfeld
    {
        ASSERT_EQ(spicesField, game->getMap()->getMapObjectFixedAt({41, 42})->getMapObjectType());

        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({41, 42}, spicesField, Direction::SOUTH);
        buildOperation.doBuild();

        ASSERT_EQ(spicesField, game->getMap()->getMapObjectFixedAt({41, 42})->getMapObjectType());
        ASSERT_EQ(980, player->coins);
        ASSERT_EQ(8, colony->getGoods(configMgr->getGood("bricks")).inventory);
    }
}
