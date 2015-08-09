#include <forward_list>
#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"
#include "map/Street.h"


/**
 * @brief Test-Fixture für das Abtesten von BuildOperation::doBuild().
 *
 * Wir gehen davon aus, dass das BuildOperationResult immer korrekt ist.
 * Der Test prüft, ob die Objekte aus `mapObjectToReplaceWith` korrekt in die Map umgesetzt worden sind.
 */
class BuildOperationDoBuildTest : public BuildOperationTest {

protected:
    Colony* colony;

protected:
    virtual void SetUp() {
        BuildOperationTest::SetUp();

        colony = game->getColony({ 43, 34 });
    }

    /**
     * @brief Assert-Helper. Prüft, ob ein Map-Objekt korrekt in die Map übernommen wurde.
     * @param mapCoords erwartete Map-Koordinaten
     * @param mapObjectType erwarteter MapObjectType
     * @param view erwartete View des MapObject
     * @param mapWidth erwartete mapWidth des MapObject
     * @param mapHeight erwartete mapHeight des MapObject
     */
    void assertCorrectBuild(const MapCoords& mapCoords, const MapObjectType* mapObjectType,
                            const FourthDirection& view, unsigned char mapWidth, unsigned char mapHeight) const {

        const MapObjectFixed* mapObjectFixed = game->getMap()->getMapObjectFixedAt(mapCoords);
        ASSERT_TRUE(mapObjectFixed != nullptr);
        ASSERT_EQ(mapCoords, mapObjectFixed->getMapCoords());
        ASSERT_EQ(mapObjectType, mapObjectFixed->getMapObjectType());
        ASSERT_EQ(view, mapObjectFixed->getView());
        ASSERT_EQ(mapWidth, mapObjectFixed->getMapWidth());
        ASSERT_EQ(mapHeight, mapObjectFixed->getMapHeight());
    }

};


/**
 * @brief Testet das Ausführen einer einfachen Bauoperation.
 */
TEST_F(BuildOperationDoBuildTest, regularBuild) {
    const MapObjectType* marketplace = configMgr->getMapObjectType("marketplace");

    // Testaufbau
    player->coins = 5000;
    colony->getGoods(configMgr->getGood("tools")).inventory = 20;
    colony->getGoods(configMgr->getGood("wood")).inventory = 40;
    colony->getGoods(configMgr->getGood("bricks")).inventory = 30;

    BuildOperation buildOperation(&context, *player);
    buildOperation.requestBuildWhenNothingInTheWay(MapCoords(45, 33), marketplace, Direction::SOUTH);
    buildOperation.requestBuildWhenNothingInTheWay(MapCoords(37, 33), marketplace, Direction::NORTH);
    buildOperation.requestBuildWhenNothingInTheWay(MapCoords(38, 37), marketplace, Direction::WEST);
    buildOperation.doBuild();

    // Testauswertung: Resourcen korrekt abgezogen
    ASSERT_EQ(4400, player->coins);
    ASSERT_EQ(8, colony->getGoods(configMgr->getGood("tools")).inventory);
    ASSERT_EQ(10, colony->getGoods(configMgr->getGood("wood")).inventory);
    ASSERT_EQ(30, colony->getGoods(configMgr->getGood("bricks")).inventory);

    // Testauswertung: Gebäude in die Map übernommen
    assertCorrectBuild({45, 33}, marketplace, Direction::SOUTH, 4, 3);
    assertCorrectBuild({37, 33}, marketplace, Direction::NORTH, 4, 3);
    assertCorrectBuild({38, 37}, marketplace, Direction::WEST, 3, 4);
}

/**
 * @brief Testet das Ausführen einer Bauoperation, die nicht komplett ausgeführt werden kann, weil das
 * Baumaterial nicht reicht.
 *
 * Baue 5 Rumbrennereien, wobei die Resourcen nur für 3 reichen
 */
TEST_F(BuildOperationDoBuildTest, partialBuildOutOfResources) {
    const MapObjectType* distillery = configMgr->getMapObjectType("distillery");

    // Testaufbau
    player->coins = 2000;
    colony->getGoods(configMgr->getGood("tools")).inventory = 11;
    colony->getGoods(configMgr->getGood("wood")).inventory = 20;
    colony->getGoods(configMgr->getGood("bricks")).inventory = 30;

    BuildOperation buildOperation(&context, *player);
    buildOperation.requestBuildWhenNothingInTheWay({39, 34}, distillery, Direction::SOUTH);
    buildOperation.requestBuildWhenNothingInTheWay({39, 37}, distillery, Direction::SOUTH);
    buildOperation.requestBuildWhenNothingInTheWay({39, 39}, distillery, Direction::SOUTH);
    buildOperation.requestBuildWhenNothingInTheWay({37, 37}, distillery, Direction::SOUTH);
    buildOperation.requestBuildWhenNothingInTheWay({37, 39}, distillery, Direction::SOUTH);
    buildOperation.doBuild();

    // Testauswertung: Resourcen korrekt abgezogen
    ASSERT_EQ(1400, player->coins);
    ASSERT_EQ(2, colony->getGoods(configMgr->getGood("tools")).inventory);
    ASSERT_EQ(14, colony->getGoods(configMgr->getGood("wood")).inventory);
    ASSERT_EQ(15, colony->getGoods(configMgr->getGood("bricks")).inventory);

    // Testauswertung: Gebäude in die Map übernommen
    assertCorrectBuild({39, 34}, distillery, Direction::SOUTH, 2, 2);
    assertCorrectBuild({39, 37}, distillery, Direction::SOUTH, 2, 2);
    assertCorrectBuild({39, 39}, distillery, Direction::SOUTH, 2, 2);
    ASSERT_TRUE(game->getMap()->getMapObjectFixedAt({37, 37}) == nullptr);
    ASSERT_TRUE(game->getMap()->getMapObjectFixedAt({37, 39}) == nullptr);
}

// TODO BUILDOPERATION overbuildBuild: Förster in die 1x1 Felder bauen (vollständig und partial)

/**
 * @brief Test, der Straßenanpassung testet, wenn die Resourcen nicht mehr reichen.
 * Beim Bau dürfen nur Ersetzungen vorgenommen werden, die angrenzen an Straßen, wofür die Resourcen reichen.
 *
 * Wir bauen einen Feldweg von (43, 44) nach (47, 44).
 * Allerdings reicht das Geld nur für zwei Kacheln.
 */
TEST_F(BuildOperationDoBuildTest, adeptStreetsOutOfResources) {
    const Map* map = game->getMap();
    const MapObjectType* farmRoad = configMgr->getMapObjectType("farm-road");
    const MapObjectType* cobbledStreet = configMgr->getMapObjectType("cobbled-street");

    // Testaufbau
    player->coins = 10;

    BuildOperation buildOperation(&context, *player);
    for (int x = 43; x <= 47; x++) {
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(x, 44), farmRoad, Direction::SOUTH);
    }
    buildOperation.doBuild();

    // Testauswertung

    // Resourcen korrekt nur für 2 Kacheln abgezogen
    ASSERT_EQ(0, player->coins);

    // Bau korrekt
    assertCorrectBuild({43, 44}, farmRoad, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_SOUTH,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({43, 44}))->streetConnections);

    assertCorrectBuild({44, 44}, farmRoad, Direction::SOUTH, 1, 1);

    for (int x = 45; x <= 47; x++) {
        ASSERT_TRUE(game->getMap()->getMapObjectFixedAt({x, 44}) == nullptr);
    }

    // partielle Anpassung an (44, 44): nur West! Für die Ost-Verbindung reichen die Resourcen nicht
    ASSERT_EQ(StreetConnections::BIT_MASK_WEST,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({44, 44}))->streetConnections);

    // Anpassung an (43, 45)
    assertCorrectBuild({43, 45}, cobbledStreet, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_NORTH,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({43, 45}))->streetConnections);

    // Keine Anpassungen an (45, 45), (47, 45), (47, 43), da die Resourcen nicht reichen
    ASSERT_EQ(0, dynamic_cast<const Street*>(map->getMapObjectFixedAt({45, 45}))->streetConnections);
    ASSERT_EQ(0, dynamic_cast<const Street*>(map->getMapObjectFixedAt({47, 45}))->streetConnections);
    ASSERT_EQ(StreetConnections::BIT_MASK_NORTH,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({47, 43}))->streetConnections);
}

/**
 * @brief Spezialtest "halbe Kreuzung"
 *
 * Wir bauen einen Feldweg von (46, 31) nach (46, 33).
 * Allerdings reicht das Geld nur für eine Kachel.
 *
 * Wir erwarten, dass beim Bau nur ein T-Stück entsteht, obwohl eine Kreuzung gezogen wurde.
 */
TEST_F(BuildOperationDoBuildTest, partialCrossroads) {
    const Map* map = game->getMap();
    const MapObjectType* farmRoad = configMgr->getMapObjectType("farm-road");

    // Testaufbau
    player->coins = 5;

    BuildOperation buildOperation(&context, *player);
    for (int y = 31; y <= 33; y++) {
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(46, y), farmRoad, Direction::SOUTH);
    }
    buildOperation.doBuild();

    // Testauswertung
    ASSERT_EQ(0, player->coins);

    assertCorrectBuild({46, 31}, farmRoad, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_SOUTH,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({46, 31}))->streetConnections);

    assertCorrectBuild({46, 32}, farmRoad, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_WEST,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({46, 32}))->streetConnections);

    ASSERT_TRUE(map->getMapObjectFixedAt({46, 33}) == nullptr);
}

/**
 * @brief Testet das Ausführen einer Bauoperation, die Straßen überbaut bzw. anpasst
 *
 * Ziehe Straßenzug von (46, 31) nach (46, 36).
 * 4 Kacheln kommen neu dazu, 2 werden überbaut (wovon nur eine was kostet).
 */
TEST_F(BuildOperationDoBuildTest, overbuildAndAdeptStreet) {
    const Map* map = game->getMap();

    const MapObjectType* farmRoad = configMgr->getMapObjectType("farm-road");
    const MapObjectType* cobbledStreet = configMgr->getMapObjectType("cobbled-street");

    // Testaufbau
    player->coins = 1000;
    colony->getGoods(configMgr->getGood("tools")).inventory = 10;
    colony->getGoods(configMgr->getGood("wood")).inventory = 20;
    colony->getGoods(configMgr->getGood("bricks")).inventory = 25;

    BuildOperation buildOperation(&context, *player);
    for (int y = 31; y <= 36; y++) {
        buildOperation.requestBuildWhenNothingInTheWay(MapCoords(46, y), cobbledStreet, Direction::SOUTH);
    }

    unsigned long mapObjectsCountBefore = game->getMap()->getMapObjects().size();
    buildOperation.doBuild();
    unsigned long mapObjectsCountAfter = game->getMap()->getMapObjects().size();

    // Testauswertung: Sicherstellen, dass die Anzahl der Map-Objekte stimmt.
    // Nicht, dass es zwei Map-Objekte auf derselben Koordinate gibt
    ASSERT_EQ(mapObjectsCountBefore + 4, mapObjectsCountAfter);

    // Testauswertung: Resourcen korrekt abgezogen (nur 5 der 6 Kacheln kosten was)
    ASSERT_EQ(975, player->coins);
    ASSERT_EQ(10, colony->getGoods(configMgr->getGood("tools")).inventory);
    ASSERT_EQ(20, colony->getGoods(configMgr->getGood("wood")).inventory);
    ASSERT_EQ(20, colony->getGoods(configMgr->getGood("bricks")).inventory);

    // Testauswertung: Straßen in die Map übernommen bzw. angepasst
    assertCorrectBuild({46, 31}, cobbledStreet, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_SOUTH,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({46, 31}))->streetConnections);

    assertCorrectBuild({46, 32}, cobbledStreet, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_EAST |
              StreetConnections::BIT_MASK_SOUTH | StreetConnections::BIT_MASK_WEST,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({46, 32}))->streetConnections);

    for (int y = 33; y <= 35; y++) {
        assertCorrectBuild({ 46, y }, cobbledStreet, Direction::SOUTH, 1, 1);
        ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_SOUTH,
                  dynamic_cast<const Street*>(map->getMapObjectFixedAt({ 46, y }))->streetConnections);
    }

    assertCorrectBuild({46, 36}, cobbledStreet, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_NORTH | StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_WEST,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({46, 36}))->streetConnections);

    assertCorrectBuild({45, 32}, farmRoad, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_EAST | StreetConnections::BIT_MASK_WEST,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({45, 32}))->streetConnections);

    assertCorrectBuild({47, 32}, farmRoad, Direction::SOUTH, 1, 1);
    ASSERT_EQ(StreetConnections::BIT_MASK_WEST,
              dynamic_cast<const Street*>(map->getMapObjectFixedAt({47, 32}))->streetConnections);
}