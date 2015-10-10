#include "GameTest.h"


class InCatchmentAreaFinderGoodsPriorityTest : public GameTest {

protected:
    Building* weavingMill;
    Building* sheepFarm1;
    Building* sheepFarm2;
    std::list<const Good*> goodsWoolOnly;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        weavingMill = dynamic_cast<Building*>(game->addStructure(
            MapCoords(31, 30), configMgr->getMapObjectType("weaving-mill1"),
            Direction::SOUTH, game->getPlayer(0)));
        sheepFarm1 = dynamic_cast<Building*>(game->addStructure(
            MapCoords(35, 35), configMgr->getMapObjectType("sheep-farm"),
            Direction::SOUTH, game->getPlayer(0)));
        sheepFarm2 = dynamic_cast<Building*>(game->addStructure(
            MapCoords(30, 35), configMgr->getMapObjectType("sheep-farm"),
            Direction::SOUTH, game->getPlayer(0)));

        const Good* wool = context.configMgr->getGood("wool");
        goodsWoolOnly.push_back(wool);
    }
};


/**
 * @brief Testet InCatchmentAreaFinder, ob das vollere Lager bevorzugt wird.
 * Webstube muss aus 2 Schaffarmen mit unterschiedlichen Lagerbestand die richtige wählen.
 */
TEST_F(InCatchmentAreaFinderGoodsPriorityTest, checkThatFullerInventoryIsFetchedFirst) {
    // Testaufbau
    sheepFarm1->productionSlots.output.inventory = 2;
    sheepFarm2->productionSlots.output.inventory = 3;

    // Testdurchführung
    CatchmentAreaIterator catchmentAreaIterator(*weavingMill, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);

    InCatchmentAreaFinderResult result = inCatchmentAreaFinder.findBuildingWithGoods(goodsWoolOnly);

    ASSERT_EQ(true, result.foundSomething);
    ASSERT_EQ(sheepFarm2, result.mapObject);
    ASSERT_EQ(MapCoords(30, 35), *result.route.crbegin());
}


/**
 * @brief Testet InCatchmentAreaFinder, ob bei gleich vollem Lager das Gebäude bevorzugt wird,
 * was länger nicht abgeholt wurde. Webstube muss aus 2 Schaffarmen die richtige wählen.
 */
TEST_F(InCatchmentAreaFinderGoodsPriorityTest, checkThatOnSameInventoryLastCollectionIsUsed) {
    // Testaufbau
    sheepFarm1->productionSlots.output.inventory = 1;
    sheepFarm2->productionSlots.output.inventory = 1;

    sheepFarm1->lastGoodsCollections = 42;
    sheepFarm2->lastGoodsCollections = 46;

    // Testdurchführung
    CatchmentAreaIterator catchmentAreaIterator(*weavingMill, false);
    InCatchmentAreaFinder inCatchmentAreaFinder(&context, &catchmentAreaIterator);

    InCatchmentAreaFinderResult result = inCatchmentAreaFinder.findBuildingWithGoods(goodsWoolOnly);

    ASSERT_EQ(true, result.foundSomething);
    ASSERT_EQ(sheepFarm1, result.mapObject);
    ASSERT_EQ(MapCoords(35, 35), *result.route.crbegin());
}