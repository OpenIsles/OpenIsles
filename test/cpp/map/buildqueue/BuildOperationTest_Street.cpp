#include <forward_list>
#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"


/**
 * @brief Testet den Bau eines Straßenzugs.
 *
 * Es wird ein Straßenzug von (40, 30) nach (47, 43) gezogen.
 */
TEST_F(BuildOperationTest, buildStreets) {

    const std::forward_list<MapCoords> mapCoordsToBuildThere = {
        {40, 30}, {40, 31}, {40, 32}, {40, 33}, {40, 34}, {40, 35}, {40, 36}, {40, 37},
        {40, 38}, {40, 39}, {40, 40}, {40, 41}, {40, 42}, {40, 43},
        {41, 43}, {42, 43}, {43, 43}, {44, 43}, {45, 43}, {46, 43}, {47, 43}
    };
    const MapObjectType* cobbledStreet = context.configMgr->getMapObjectType("cobbled-street");

    // Testaufbau

    BuildOperation buildOperation(&context, *player);
    for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
        buildOperation.requestBuild(mapCoords, cobbledStreet, Direction::SOUTH);
    }

    const BuildOperationResult& result = buildOperation.getResult();

    // Testauswertung

    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(21, result.size());

    // Für alle Kacheln muss gelten...
    for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
        const BuildOperationResultBit& resultBit = *result.at(mapCoords);

        ASSERT_EQ(false, resultBit.somethingInTheWay);
        ASSERT_EQ(false, resultBit.deleteMapObjectThere);

        ASSERT_EQ(cobbledStreet, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(Direction::SOUTH, resultBit.mapObjectToReplaceWith->getView()); // Immer SOUTH bei Straßen
        ASSERT_TRUE(resultBit.mapObjectToReplaceWith == resultBit.mapObjectToDraw);
    }

    // Geraden-Ende oben
    {
        const BuildOperationResultBit& resultBit = *result.at({40, 30});
        ASSERT_EQ(StreetConnections::BIT_MASK_SOUTH,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // Geraden vertikal
    for (const int y : {31, 33, 34, 35, 37, 38, 39, 40, 41, 42}) {
        const BuildOperationResultBit& resultBit = *result.at({40, y});
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_SOUTH,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // Geraden horizontal
    for (const int x : {41, 42, 43, 44, 45, 46}) {
        const BuildOperationResultBit& resultBit = *result.at({x, 43});
        ASSERT_EQ(StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_WEST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // T-Stücke (Umwandlung: Straight-Feldweg zu T-Feldweg)
    for (const int y : {32, 36}) {
        const BuildOperationResultBit& resultBit = *result.at({40, y});
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_SOUTH,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // Kurve (neu gebaut)
    {
        const BuildOperationResultBit& resultBit = *result.at({40, 43});
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_EAST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // Kurve (Umwandlung: Straight-Straße zu Kurve-Straße)
    {
        const BuildOperationResultBit& resultBit = *result.at({47, 43});
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_WEST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(true, resultBit.costsNothingBecauseOfChange);
    }
}

/**
 * @brief Testet, ob für Gratis-Überbauen die Resourcen korrekt berücksichtigt werden.
 *
 * Es wird ein Straßenzugen von 5 Feldern über Straßenzug von 4 Feldern + 1 Feldweg gebaut:
 * Von (40, 36) nach (44, 36) gezogen.
 * Resourcen sind für 2 Straßen übrig. Die Resourcen müssen reichen (weil 4 Felder gratis sind).
 */
TEST_F(BuildOperationTest, costsNothingOverbuildResources) {

    const MapObjectType* cobbledStreet = context.configMgr->getMapObjectType("cobbled-street");

    // Testaufbau

    BuildOperation buildOperation(&context, *player);
    for (int x = 40; x <= 44; x++) {
        buildOperation.requestBuild({x, 36}, cobbledStreet, Direction::SOUTH);
    }

    Colony* colony = game->getColony({ 40, 36 });
    colony->getGoods(configMgr->getGood("bricks")).inventory = 2;

    const BuildOperationResult& result = buildOperation.getResult();

    // Testauswertung

    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(5, result.size());

    const BuildOperationResultBit& resultBit = *result.at({40, 36});
    ASSERT_EQ(cobbledStreet, resultBit.mapObjectToReplaceWith->getMapObjectType());
    ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    ASSERT_EQ(true, resultBit.resourcesEnoughToBuildThis);

    for (int x = 41; x <= 44; x++) {
        const BuildOperationResultBit& resultBit = *result.at({x, 36});

        ASSERT_EQ(cobbledStreet, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(true, resultBit.costsNothingBecauseOfChange);
        ASSERT_EQ(true, resultBit.resourcesEnoughToBuildThis);
    }
}

/**
 * @brief Testet, ob für einzelne Straßenkacheln die View korrekt ist.
 */
TEST_F(BuildOperationTest, buildLonelyStreet) {

    const MapObjectType* farmRoad = context.configMgr->getMapObjectType("farm-road");

    // Testaufbau

    BuildOperation buildOperation(&context, *player);
    buildOperation.requestBuild({46, 34}, farmRoad, Direction::SOUTH);
    buildOperation.requestBuild({48, 34}, farmRoad, Direction::WEST);
    buildOperation.requestBuild({50, 34}, farmRoad, Direction::EAST);

    const BuildOperationResult& result = buildOperation.getResult();

    // Testauswertung

    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(3, result.size());

    {
        const BuildOperationResultBit& resultBit = *result.at({46, 34});
        ASSERT_EQ(Direction::SOUTH, resultBit.mapObjectToReplaceWith->getView());
    }

    {
        const BuildOperationResultBit& resultBit = *result.at({48, 34});
        ASSERT_EQ(Direction::WEST, resultBit.mapObjectToReplaceWith->getView());
    }

    {
        const BuildOperationResultBit& resultBit = *result.at({50, 34});
        ASSERT_EQ(Direction::EAST, resultBit.mapObjectToReplaceWith->getView());
    }
}

/**
 * @brief Testet, ob vorhandene Straßenkacheln sich korrekt verhalten, wenn sie mit einer
 * anderen Straße verbunden werden.
 *
 * Es wird ein Straßenzug von (43, 44) nach (49, 44) gezogen.
 */
TEST_F(BuildOperationTest, adeptStreets) {

    const MapObjectType* farmRoad = context.configMgr->getMapObjectType("farm-road");
    const MapObjectType* cobbledStreet = context.configMgr->getMapObjectType("cobbled-street");

    // Testaufbau

    BuildOperation buildOperation(&context, *player);
    for (int x = 43; x <= 49; x++) {
        buildOperation.requestBuild({x, 44}, farmRoad, Direction::EAST);
    }

    const BuildOperationResult& result = buildOperation.getResult();

    // Testauswertung

    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(12, result.size()); // 7 neue + 5 vorhandene

    // neu verlegte Kacheln: Kurve links
    {
        const BuildOperationResultBit& resultBit = *result.at({43, 44});
        ASSERT_EQ(farmRoad, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_SOUTH,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // neu verlegte Kacheln: Geraden
    for (int x = 44; x <= 48; x += 2) {
        const BuildOperationResultBit& resultBit = *result.at({x, 44});
        ASSERT_EQ(farmRoad, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_WEST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // neu verlegte Kacheln: T-Stück
    {
        const BuildOperationResultBit& resultBit = *result.at({45, 44});
        ASSERT_EQ(farmRoad, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_SOUTH | StreetConnections::BIT_MASK_WEST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // neu verlegte Kacheln: Kreuzung
    {
        const BuildOperationResultBit& resultBit = *result.at({47, 44});
        ASSERT_EQ(farmRoad, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_EAST |
                      StreetConnections::BIT_MASK_SOUTH | StreetConnections::BIT_MASK_WEST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // neu verlegte Kacheln: Kurve rechts
    {
        const BuildOperationResultBit& resultBit = *result.at({49, 44});
        ASSERT_EQ(farmRoad, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(StreetConnections::BIT_MASK_SOUTH | StreetConnections::BIT_MASK_WEST,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);
    }

    // bestehende Kacheln
    const std::forward_list<MapCoords> mapCoordsToAdept = { {47, 43}, {43, 45}, {45, 45}, {47, 45}, {49, 45} };
    for (const MapCoords& mapCoords : mapCoordsToAdept) {
        const BuildOperationResultBit& resultBit = *result.at(mapCoords);
        ASSERT_EQ(cobbledStreet, resultBit.mapObjectToReplaceWith->getMapObjectType());
        ASSERT_EQ(Direction::SOUTH, resultBit.mapObjectToReplaceWith->getView());
        ASSERT_EQ(true, resultBit.costsNothingBecauseOfChange);
    }
    for (int x = 43; x <= 49; x += 2) {
        const BuildOperationResultBit& resultBit = *result.at({x, 45});
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
    }
    {
        const BuildOperationResultBit& resultBit = *result.at({47, 43});
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_SOUTH,
                  std::dynamic_pointer_cast<const Street>(resultBit.mapObjectToReplaceWith)->streetConnections);
    }
}



///**
// * @brief Überprüft verschiedene Kombinationen mit Überbauen
// * - Feldweg über Straße (muss gehen)
// * - Straße über Feldweg (muss gehen)
// * - Straße über Straße (muss ignoriert werden)
// * - Straße über Platz (muss ignoriert werden)
// */
//TEST_F(BuildOperationTest, buildStreetOverStreet) {
//
//}
