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
 * @brief Überprüft, ob man die Objekte in der richtigen Reihenfolge wieder aus der Liste kriegt
 */
TEST_F(BuildOperationTest, orderOfInsertion) {
    const MapObjectType* pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");

    BuildOperation buildOperation(&context, *game->getCurrentPlayer());

    buildOperation.requestBuild({36, 37}, pioneersHouse1, Direction::EAST);
    buildOperation.requestBuild({39, 40}, pioneersHouse1, Direction::EAST);
    buildOperation.requestBuild({36, 39}, pioneersHouse1, Direction::EAST);
    buildOperation.requestBuild({39, 34}, pioneersHouse1, Direction::EAST);

    const std::list<MapObjectToBuild>& mapObjectsToBuild = buildOperation.getMapObjectsToBuild();
    auto iter = mapObjectsToBuild.cbegin();

    ASSERT_EQ(MapCoords(36, 37), iter->mapCoords);
    iter++;
    ASSERT_EQ(MapCoords(39, 40), iter->mapCoords);
    iter++;
    ASSERT_EQ(MapCoords(36, 39), iter->mapCoords);
    iter++;
    ASSERT_EQ(MapCoords(39, 34), iter->mapCoords);
    iter++;
}


/**
 * @brief Überprüft, ob korrekt erkannt wird, wenn was im Weg is
 */
TEST_F(BuildOperationTest, somethingInTheWay) {
    const MapObjectType* tavern = configMgr->getMapObjectType("tavern");

    {
        // Wirtshaus normal einsetzen: genug Platz
        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        buildOperation.requestBuild(MapCoords(45, 33), tavern, Direction::SOUTH);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::OK, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(false, result.at(MapCoords(46, 35))->somethingInTheWay);
    }

    {
        // Wirtshaus gedreht einsetzen: genug Platz
        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        buildOperation.requestBuild(MapCoords(45, 33), tavern, Direction::WEST);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::OK, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(false, result.at(MapCoords(47, 34))->somethingInTheWay);
    }

    {
        // Wirtshaus normal einsetzen: ein Feldweg im Weg
        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        buildOperation.requestBuild(MapCoords(39, 34), tavern, Direction::NORTH);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::SOMETHING_IN_THE_WAY, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(true, result.at(MapCoords(40, 35))->somethingInTheWay);
    }

    {
        // Wirtshaus gedreht einsetzen: ein Feldweg im Weg
        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
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

    {
        // Ersten Marktplatz setzen
        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(45, 33), marketplace, Direction::SOUTH);
        ASSERT_EQ(12, buildOperation.getResult().size());

        // Weiteren Marktplatz überlappen -> es darf sich nix tun
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(48, 32), marketplace, Direction::WEST);
        ASSERT_EQ(12, buildOperation.getResult().size());

        // Weiteren Marktplatz überlappen über Bauqueue und bestehendes Objekt  -> es darf sich auch nix tun
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(43, 31), marketplace, Direction::WEST);
        ASSERT_EQ(12, buildOperation.getResult().size());

        // Weiteren Marktplatz korrekt hinzufügen
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(49, 33), marketplace, Direction::EAST);
        ASSERT_EQ(24, buildOperation.getResult().size());
    }

    {
        const MapCoords mapCoords(47, 39);

        // Mit requestBuildWhenNothingInTheWay darf ich auf der Karte nicht überlappen
        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        buildOperation.requestBuildWhenNothingInTheWay(mapCoords, marketplace, Direction::SOUTH);
        ASSERT_EQ(0, buildOperation.getResult().size());

        // Mit requestBuild schon
        buildOperation.requestBuild(mapCoords, marketplace, Direction::SOUTH);
        ASSERT_EQ(12, buildOperation.getResult().size());
    }

}

/**
 * @brief Überprüft, ob die Baukosten korrekt berücksichtigt werden
 */
TEST_F(BuildOperationTest, buildingCosts) {
    const MapObjectType* toolsmiths = configMgr->getMapObjectType("toolsmiths");
    Colony* colony = game->getColony({ 43, 34 });

    {
        game->getCurrentPlayer()->coins = 500;
        colony->getGoods(configMgr->getGood("tools")).inventory = 6;
        colony->getGoods(configMgr->getGood("wood")).inventory = 30;
        colony->getGoods(configMgr->getGood("bricks")).inventory = 20;

        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        const MapCoords mc1 = { 45, 34 };
        const MapCoords mc2 = { 47, 34 };
        const MapCoords mc3 = { 39, 37 };

        // Erste Werkzeugschmiede setzen: Alle Rohstoffe reichen
        buildOperation.requestBuildWhenNothingInTheWay(mc1, toolsmiths, Direction::SOUTH);
        ASSERT_EQ(true, buildOperation.getResult().at(mc1)->resourcesEnoughToBuildThis);

        // Zweite Werkzeugschmiede hinzufügen: Rohstoffe reichen immer noch. Werkzeuge reichen exakt
        buildOperation.requestBuildWhenNothingInTheWay(mc2, toolsmiths, Direction::SOUTH);
        ASSERT_EQ(true, buildOperation.getResult().at(mc1)->resourcesEnoughToBuildThis);
        ASSERT_EQ(true, buildOperation.getResult().at(mc2)->resourcesEnoughToBuildThis);

        // Dritte Werkzeugschmiede hinzufügen: Werkzeuge reichen nicht mehr
        buildOperation.requestBuildWhenNothingInTheWay(mc3, toolsmiths, Direction::SOUTH);
        ASSERT_EQ(true, buildOperation.getResult().at(mc1)->resourcesEnoughToBuildThis);
        ASSERT_EQ(true, buildOperation.getResult().at(mc2)->resourcesEnoughToBuildThis);
        ASSERT_EQ(false, buildOperation.getResult().at(mc3)->resourcesEnoughToBuildThis);
    }

    {
        // Geld reicht nicht
        game->getCurrentPlayer()->coins = 100;
        colony->getGoods(configMgr->getGood("tools")).inventory = 3;
        colony->getGoods(configMgr->getGood("wood")).inventory = 2;
        colony->getGoods(configMgr->getGood("bricks")).inventory = 5;

        BuildOperation buildOperation(&context, *game->getCurrentPlayer());
        buildOperation.requestBuildWhenNothingInTheWay({38, 40}, toolsmiths, Direction::SOUTH);
        ASSERT_EQ(false, buildOperation.getResult().at({38, 40})->resourcesEnoughToBuildThis);
    }
}

/**
 * @brief Überprüft, dass wir auch mit einer leeren Buildqueue zurechtkommen.
 */
TEST_F(BuildOperationTest, emptyBuildQueue) {
    const MapObjectType* pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");

    BuildOperation buildOperation(&context, *game->getCurrentPlayer());
    buildOperation.requestBuildWhenNothingInTheWay({47, 39}, pioneersHouse1, Direction::NORTH); // da is belegt

    buildOperation.updateBuildMaterials(); // rebuildResult() triggern

    ASSERT_TRUE(buildOperation.getMapObjectsToBuild().empty());
    ASSERT_TRUE(buildOperation.getResult().empty());
    ASSERT_EQ(BuildOperationResult::OK, buildOperation.getResult().result);
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