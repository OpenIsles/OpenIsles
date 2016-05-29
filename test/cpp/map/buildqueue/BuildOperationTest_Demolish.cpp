#include <initializer_list>
#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"


/**
 * @brief Testet das Abreißen eines Gebäudes
 *
 * Wir reißen das Wirtshaus auf (48, 37) ab.
 */
TEST_F(BuildOperationTest, demolishBuilding) {
    BuildingCosts resourcesBefore = BuildingCosts(player, colony, context.configMgr);

    const Map* map = context.game->getMap();
    const MapObjectFixed* tavern = map->getMapObjectFixedAt({48, 37});
    ASSERT_TRUE(tavern != nullptr);

    // Test 1: BuildOperation
    BuildOperation buildOperation(context, *player);
    buildOperation.requestDemolish(*tavern);

    ASSERT_EQ(MapObjectToBuild::DEMOLISH, buildOperation.getMapObjectsToBuildMode());
    ASSERT_EQ(colony, buildOperation.getColony());

    const BuildOperationResult& result = buildOperation.getResult();
    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(6, result.size());

    std::initializer_list<MapCoords> allMapCoordsTavern = {{48, 37}, {49, 37}, {50, 37}, {48, 38}, {49, 38}, {50, 38}};
    for (const MapCoords& mapCoords : allMapCoordsTavern) {
        const BuildOperationResultBit& resultBit = *result.at(mapCoords);

        ASSERT_EQ(true, resultBit.deleteMapObjectThere);

        // immer false beim Abreißen
        ASSERT_EQ(false, resultBit.somethingInTheWay);
        ASSERT_EQ(false, resultBit.resourcesEnoughToBuildThis);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);

        ASSERT_TRUE(resultBit.mapObjectToReplaceWith == nullptr);
        ASSERT_TRUE(resultBit.mapObjectToDraw == nullptr);
    }

    // Test 2: Ausführen
    buildOperation.doBuild();

    // Testauswertung: Gebäude muss weg sein
    ASSERT_TRUE(map->getMapObjectFixedAt({48, 37}) == nullptr);

    // Testauswertung: Abreißen darf nix gekostet haben
    BuildingCosts resourcesAfter = BuildingCosts(player, colony, context.configMgr);
    ASSERT_TRUE(resourcesAfter == resourcesBefore);
}