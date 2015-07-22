#include <forward_list>
#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"


/**
 * @brief Test-Fixture, um die BuildOperations zu testen.
 *
 * Testaufbau:
 * @image html buildoperation-test.png
 */
class BuildOperationTest : public GameTest {

protected:
    BuildOperationTest() : GameTest("buildqueue-testmap.tmx") {
    }
};


/**
 * @brief Überprüft, ob beim Bauauftrag von Straßen
 * a) neue Kacheln korrekt konvertiert werden (z.&nbsp;B. eine Kurve gesetzt wird)
 * b) vorhandene Kacheln als zu ersetzen auszeichnet werden (z.&nbsp;B. wird aus einer Gerade ein T-Stück, wenn
 *    eine Abzweigung dazukommt)
 *
 * Es wird ein Straßenzug von (40, 30) nach (47, 43) gezogen.
 */
//TEST_F(BuildOperationTest, tilesCorrectlyFixedForStreets) {
//    // Notwendige Variablen
//
//    const std::forward_list<MapCoords> mapCoordsToBuildThere = {
//        {40, 30}, {40, 31}, {40, 32}, {40, 33}, {40, 34}, {40, 35}, {40, 36}, {40, 37},
//        {40, 38}, {40, 39}, {40, 40}, {40, 41}, {40, 42}, {40, 43},
//        {41, 43}, {42, 43}, {43, 43}, {44, 43}, {45, 43}, {46, 43}, {47, 43}
//    };
//    const MapObjectType* streetType = context.configMgr->getMapObjectType("cobbled-street-straight0");
//
//
//    const MapObjectFixed* office1 = game->getMap()->getMapObjectFixedAt(MapCoords(41, 33));
//    const Colony* colony = game->getColony(office1);
//    const Player* player = office1->getPlayer();
//    ASSERT_TRUE(colony != nullptr);
//
//    // Testaufbau
//
//    BuildOperation buildOperation(*colony, *player, *game);
//    for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
//        buildOperation.requestBuild(mapCoords, streetType, Direction::SOUTH);
//    }
//
//    const BuildOperationResult& result = buildOperation.getResult();
//
//    // Testauswertung
//
//    ASSERT_EQ(BuildOperationResult::OK, result.result);
//    ASSERT_EQ(21, result.size());
//
//    for (const int y : {30, 31, 33, 34, 35, 37, 38, 39, 40, 41, 42}) {
//        const BuildOperationResultBit& resultBit = result.at({ 40, y });
//        ASSERT_EQ(true, resultBit.resourcesEnoughToBuildThis);
//    }
//
//}


/**
 * @brief Überprüft, ob korrekt erkannt wird, wenn was im Weg is
 */
TEST_F(BuildOperationTest, somethingInTheWay) {
    const MapObjectType* tavern = configMgr->getMapObjectType("tavern");

    {
        // Wirtshaus normal einsetzen: genug Platz
        BuildOperation buildOperation(*game->getCurrentPlayer(), *game);
        buildOperation.requestBuild(MapCoords(45, 33), tavern, Direction::SOUTH);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::OK, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(false, result.at(MapCoords(46, 35))->somethingInTheWay);
    }

    {
        // Wirtshaus gedreht einsetzen: genug Platz
        BuildOperation buildOperation(*game->getCurrentPlayer(), *game);
        buildOperation.requestBuild(MapCoords(45, 33), tavern, Direction::WEST);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::OK, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(false, result.at(MapCoords(47, 34))->somethingInTheWay);
    }

    {
        // Wirtshaus normal einsetzen: ein Feldweg im Weg
        BuildOperation buildOperation(*game->getCurrentPlayer(), *game);
        buildOperation.requestBuild(MapCoords(39, 34), tavern, Direction::NORTH);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::SOMETHING_IN_THE_WAY, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(true, result.at(MapCoords(40, 35))->somethingInTheWay);
    }

    {
        // Wirtshaus gedreht einsetzen: ein Feldweg im Weg
        BuildOperation buildOperation(*game->getCurrentPlayer(), *game);
        buildOperation.requestBuild(MapCoords(38, 36), tavern, Direction::EAST);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::SOMETHING_IN_THE_WAY, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(true, result.at(MapCoords(40, 37))->somethingInTheWay);
    }
}

/**
 * @brief Überprüft, ob Überlappungen beim Hinzufügen von Bauaufträgen korrekt erkannt werden.
 */
TEST_F(BuildOperationTest, overlappingRequestBuild) {
    const MapObjectType* marketplace = configMgr->getMapObjectType("marketplace");

    // Ersten Marktplatz setzen
    BuildOperation buildOperation(*game->getCurrentPlayer(), *game);
    buildOperation.requestBuild(MapCoords(45, 33), marketplace, Direction::SOUTH);
    ASSERT_EQ(12, buildOperation.getResult().size());

    // Weiteren Marktplatz überlappen -> es darf sich nix tun
    buildOperation.requestBuild(MapCoords(48, 32), marketplace, Direction::WEST);
    ASSERT_EQ(12, buildOperation.getResult().size());

    // Weiteren Marktplatz überlappen über Bauqueue und bestehendes Objekt  -> es darf sich auch nix tun
    buildOperation.requestBuild(MapCoords(43, 31), marketplace, Direction::WEST);
    ASSERT_EQ(12, buildOperation.getResult().size());

    // Weiteren Marktplatz korrekt hinzufügen
    buildOperation.requestBuild(MapCoords(49, 33), marketplace, Direction::EAST);
    ASSERT_EQ(24, buildOperation.getResult().size());
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