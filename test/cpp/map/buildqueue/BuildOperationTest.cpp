#include <forward_list>
#include <numeric>
#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"


/**
 * @brief Überprüft, ob man die Objekte in der richtigen Reihenfolge wieder aus der Liste kriegt
 */
TEST_F(BuildOperationTest, orderOfInsertion) {
    const MapObjectType* pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");

    BuildOperation buildOperation(&context, *player);

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
    ASSERT_TRUE(iter == mapObjectsToBuild.cend());
}

/**
 * @brief Überprüft, ob korrekt erkannt wird, wenn was im Weg is
 */
TEST_F(BuildOperationTest, somethingInTheWay) {
    const MapObjectType* tavern = configMgr->getMapObjectType("tavern");

    {
        // Wirtshaus normal einsetzen: genug Platz
        BuildOperation buildOperation(&context, *player);
        buildOperation.requestBuild(MapCoords(45, 33), tavern, Direction::SOUTH);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::OK, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(false, result.at(MapCoords(46, 35))->somethingInTheWay);
    }

    {
        // Wirtshaus gedreht einsetzen: genug Platz
        BuildOperation buildOperation(&context, *player);
        buildOperation.requestBuild(MapCoords(45, 33), tavern, Direction::WEST);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::OK, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(false, result.at(MapCoords(47, 34))->somethingInTheWay);
    }

    {
        // Wirtshaus normal einsetzen: ein Feldweg im Weg
        BuildOperation buildOperation(&context, *player);
        buildOperation.requestBuild(MapCoords(39, 34), tavern, Direction::NORTH);
        const BuildOperationResult& result = buildOperation.getResult();
        ASSERT_EQ(BuildOperationResult::SOMETHING_IN_THE_WAY, result.result);
        ASSERT_EQ(6, result.size());
        ASSERT_EQ(true, result.at(MapCoords(40, 35))->somethingInTheWay);
    }

    {
        // Wirtshaus gedreht einsetzen: ein Feldweg im Weg
        BuildOperation buildOperation(&context, *player);
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
        BuildOperation buildOperation(&context, *player);
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
        BuildOperation buildOperation(&context, *player);
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
        player->coins = 500;
        colony->getGoods(configMgr->getGood("tools")).inventory = 6;
        colony->getGoods(configMgr->getGood("wood")).inventory = 30;
        colony->getGoods(configMgr->getGood("bricks")).inventory = 20;

        BuildOperation buildOperation(&context, *player);
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
        player->coins = 100;
        colony->getGoods(configMgr->getGood("tools")).inventory = 3;
        colony->getGoods(configMgr->getGood("wood")).inventory = 2;
        colony->getGoods(configMgr->getGood("bricks")).inventory = 5;

        BuildOperation buildOperation(&context, *player);
        buildOperation.requestBuildWhenNothingInTheWay({38, 40}, toolsmiths, Direction::SOUTH);
        ASSERT_EQ(false, buildOperation.getResult().at({38, 40})->resourcesEnoughToBuildThis);
    }
}

/**
 * @brief Überprüft, dass wir auch mit einer leeren Buildqueue zurechtkommen.
 */
TEST_F(BuildOperationTest, emptyBuildQueue) {
    const MapObjectType* pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");

    BuildOperation buildOperation(&context, *player);
    buildOperation.requestBuildWhenNothingInTheWay({47, 39}, pioneersHouse1, Direction::NORTH); // da is belegt

    buildOperation.updateBuildMaterials(); // rebuildResult() triggern

    ASSERT_TRUE(buildOperation.getMapObjectsToBuild().empty());
    ASSERT_TRUE(buildOperation.getResult().empty());
    ASSERT_EQ(BuildOperationResult::OK, buildOperation.getResult().result);
}

/**
 * @brief Überprüft, dass Häuser beim Enqueue in die BuildOperation zufällig gewählt werden.
 * Wir bauen 2x dasselbe und wollen ein anderes Ergebnis haben.
 */
TEST_F(BuildOperationTest, randomizeWhenBuildingHouses) {
    // Testaufbau

    const unsigned char HOUSE_TYPES_COUNT = 5;
    const MapObjectType* pioneersHouses[HOUSE_TYPES_COUNT] = {
        configMgr->getMapObjectType("pioneers-house1"),
        configMgr->getMapObjectType("pioneers-house2"),
        configMgr->getMapObjectType("pioneers-house3"),
        configMgr->getMapObjectType("pioneers-house4"),
        configMgr->getMapObjectType("pioneers-house5")
    };

    const std::forward_list<MapCoords> mapCoordsToBuildThere = {
        {38, 32}, {38, 34}, {38, 36}, {38, 38}, {38, 40}, {38, 42},
        {36, 33}, {36, 35}, {36, 37}, {36, 39},
        {45, 33}, {47, 34}, {49, 33}, {51, 35}, {51, 37}, {52, 39}, {54, 39}
    };

    Colony* colony = game->getColony({ 43, 34 });
    colony->getGoods(configMgr->getGood("wood")).inventory = 60;

    // Testdurchführung: Baue zweimal dasselbe und vergleiche das Ergebnis

    auto doOneBuild = [&]() {
        BuildOperation buildOperation(&context, *player);
        for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
            buildOperation.requestBuildWhenNothingInTheWay(mapCoords, pioneersHouses[0], Direction::SOUTH);
        }
        return buildOperation.getResult();
    };

    BuildOperationResult result1 = doOneBuild();
    BuildOperationResult result2 = doOneBuild();

    // Testauswertung

    ASSERT_EQ(68, result1.size()); // Beide Ergebnis müssen 17 Häuser haben
    ASSERT_EQ(68, result2.size());

    unsigned int differences = 0;
    unsigned int pioneersHouse1CountInResult1 = 0;
    unsigned int pioneersHouse1CountInResult2 = 0;
    for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
        const MapObjectType* mapObjectType1 = result1.at(mapCoords)->mapObjectToReplaceWith->getMapObjectType();
        const MapObjectType* mapObjectType2 = result2.at(mapCoords)->mapObjectToReplaceWith->getMapObjectType();

        if (mapObjectType1 != mapObjectType2) {
            differences++;
        }
        if (mapObjectType1 == pioneersHouses[0]) {
            pioneersHouse1CountInResult1++;
        }
        if (mapObjectType2 == pioneersHouses[0]) {
            pioneersHouse1CountInResult2++;
        }
    }

    ASSERT_NE(pioneersHouse1CountInResult1, 68); // Die Häuser dürfen nicht alle pioneersHouse1 sein
    ASSERT_NE(pioneersHouse1CountInResult2, 68);

    ASSERT_GT(differences, 0); // Die Häuser dürfen nicht alle identisch sein
}

/**
 * @brief Überprüft, dass beim Bau von Häusern sich bereits bestehende Häuser nicht ändern, wenn wir
 * ein weiteres hinzufügen.
 */
TEST_F(BuildOperationTest, addingOneMoreHouse) {
    // Testaufbau

    const MapObjectType* pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");
    const std::forward_list<MapCoords> mapCoordsToBuildThere = {
        {38, 32}, {38, 34}, {38, 36}, {38, 38}, {38, 40}, {38, 42},
        {36, 33}, {36, 35}, {36, 37}, {36, 39},
        {45, 33}, {47, 34}, {49, 33}, {51, 35}, {51, 37}, {52, 39}, {54, 39}
    };

    Colony* colony = game->getColony({ 43, 34 });
    colony->getGoods(configMgr->getGood("wood")).inventory = 65;

    // Testdurchführung: Baue viele Häuser, baue danach ein weiteres. Es dürfen sich bestehende nicht ändern.

    BuildOperation buildOperation(&context, *player);
    for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
        buildOperation.requestBuildWhenNothingInTheWay(mapCoords, pioneersHouse1, Direction::SOUTH);
    };
    BuildOperationResult resultBefore = buildOperation.getResult();

    const MapCoords mapCoordsToBuildAfter = {43, 29};
    buildOperation.requestBuildWhenNothingInTheWay(mapCoordsToBuildAfter, pioneersHouse1, Direction::SOUTH);
    BuildOperationResult resultAfter = buildOperation.getResult();

    // Testauswertung

    ASSERT_EQ(68, resultBefore.size());
    ASSERT_EQ(72, resultAfter.size());

    for (const MapCoords& mapCoords : mapCoordsToBuildThere) {
        const MapObjectType* mapObjectTypeBefore =
            resultBefore.at(mapCoords)->mapObjectToReplaceWith->getMapObjectType();
        const MapObjectType* mapObjectTypeAfter =
            resultAfter.at(mapCoords)->mapObjectToReplaceWith->getMapObjectType();

        ASSERT_EQ(mapObjectTypeBefore, mapObjectTypeAfter) << "mapCoords = " << ::testing::PrintToString(mapCoords);
    }

    ASSERT_TRUE(resultAfter.at(mapCoordsToBuildAfter)->mapObjectToReplaceWith != nullptr);
}