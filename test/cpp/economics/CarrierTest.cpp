#include "GameTest.h"


class CarrierTest : public GameTest {

protected:
    Building* weavingMill;
    Building* sheepFarm1;
    Building* sheepFarm2;

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

        // keine Wartezeit am Träger einstellen
        weavingMill->nextCarrierMinTicks = 0;

        ASSERT_TRUE(weavingMill->carriers.empty());
    }
};


/**
 * @brief Testet, ob die Webstube einen Träger zur Schaffarm mit dem volleren Lager schickt
 */
TEST_F(CarrierTest, checkThatCarrierIsSentToFullerInventoryFirst) {
    // Testaufbau
    sheepFarm1->productionSlots.output.inventory = 2;
    sheepFarm2->productionSlots.output.inventory = 3;

    // Testdurchführung
    game->update(500);
    ASSERT_EQ(1, weavingMill->carriers.size());

    const Carrier* carrier = *weavingMill->carriers.cbegin();
    ASSERT_EQ(MapCoords(30, 35), *carrier->route.crbegin());
}


/**
 * @brief Testet, ob bei gleich vollem Lager der Träger zum Gebäude schickt,
 * was länger nicht abgeholt wurde. Webstube muss aus 2 Schaffarmen die richtige wählen.
 */
TEST_F(CarrierTest, checkThatCarrierIsSentByLastCollectionWhenSameInventory) {
    // Testaufbau
    sheepFarm1->productionSlots.output.inventory = 1;
    sheepFarm2->productionSlots.output.inventory = 1;

    sheepFarm1->lastGoodsCollections = 42;
    sheepFarm2->lastGoodsCollections = 46;

    // Testdurchführung
    game->update(500);
    ASSERT_EQ(1, weavingMill->carriers.size());

    const Carrier* carrier = *weavingMill->carriers.cbegin();
    ASSERT_EQ(MapCoords(35, 35), *carrier->route.crbegin());
}