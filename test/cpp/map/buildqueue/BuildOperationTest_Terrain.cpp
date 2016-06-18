#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"


/**
 * @brief Testet, dass Straßen nur auf das passende Terrain gebaut werden dürfen
 */
TEST_F(BuildOperationTest, testTerrain_street) {
    const MapObjectType* cobbledStreet = context.configMgr->getMapObjectType("cobbled-street");

    // Bau auf Wasser
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({29, 22}, cobbledStreet, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Wasser)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({31, 20}, cobbledStreet, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Sand)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({34, 20}, cobbledStreet, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Gras)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({36, 22}, cobbledStreet, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Wiese
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({33, 23}, cobbledStreet, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::OK, buildOperation.getResult().result);
    }
}

/**
 * @brief Testet, dass Häuser nur auf das passende Terrain gebaut werden dürfen.
 * Häuser sind 2x2 groß. Dieser Test prüft, wenn ein Haus über mehrere Terrains geht.
 */
TEST_F(BuildOperationTest, testTerrain_house) {
    const MapObjectType* settlersHouse1 = context.configMgr->getMapObjectType("settlers-house1");

    // Bau auf Wasser / Küste
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({30, 20}, settlersHouse1, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Sand / Hang)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({35, 21}, settlersHouse1, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Wiese, Teil Küste
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({36, 22}, settlersHouse1, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Wiese, komplett
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({37, 22}, settlersHouse1, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::OK, buildOperation.getResult().result);
    }
}

/**
 * @brief Testet, dass Anlegestellen nur auf das passende Terrain gebaut werden dürfen.
 */
TEST_F(BuildOperationTest, testTerrain_pier) {
    const MapObjectType* pier = context.configMgr->getMapObjectType("pier");

    // Bau auf Wasser
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({29, 22}, pier, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Wasser)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({31, 20}, pier, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Sand)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({34, 20}, pier, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::OK, buildOperation.getResult().result);
    }

    // Bau auf Küste (Gras)
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({36, 22}, pier, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::OK, buildOperation.getResult().result);
    }

    // Bau auf Wiese
    {
        BuildOperation buildOperation(context, *player);
        buildOperation.requestBuild({33, 23}, pier, Direction::SOUTH);
        ASSERT_EQ(BuildOperationResult::NOT_OK, buildOperation.getResult().result);
    }
}