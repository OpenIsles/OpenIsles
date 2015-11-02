#include <map/MapObjectType.h>
#include "GameTest.h"


class EconomicsMgrTest : public GameTest {

protected:
    Building* sheepFarm;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        FourthDirection southView = Direction::SOUTH;
        Player* player = game->getPlayer(0);
        MapObjectType* sheepFarmType = configMgr->getMapObjectType("sheep-farm");

        sheepFarm = dynamic_cast<Building*>(game->addStructure(
            MapCoords(26, 27), sheepFarmType, southView, player));

        // Keine Schafe losschicken, damit der Test die input-Güter selbst kontrollieren kann
        sheepFarmType->maxCarriers = 0;

        ASSERT_EQ(30, sheepFarmType->secondsToProduce);
    }

};


/**
 * @brief Prüft, dass eine Schaffarm, die kein Grass hat, auch wirklich nichts herstellt.
 */
TEST_F(EconomicsMgrTest, checkThatSheepFarmWithoutInputGoodsDoesntProduceAnything) {
    sheepFarm->productionSlots.input.inventory = 0;
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);

    game->update(60000, 5000);

    ASSERT_EQ(0, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);
}

/**
 * @brief Prüft, dass die Schaffarm Gras zu Wolle umwandelt
 */
TEST_F(EconomicsMgrTest, checkThatSheepFarmProducesWoolOutOfGrass) {
    sheepFarm->productionSlots.input.inventory = 8;
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);

    // Produktion alle 30 Sekunden
    game->update(29000, 1000);

    ASSERT_EQ(8, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);

    game->update(1000);

    ASSERT_EQ(4, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(1, sheepFarm->productionSlots.output.inventory);

    // Zweite Produktion
    game->update(30000, 1000);

    ASSERT_EQ(0, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(2, sheepFarm->productionSlots.output.inventory);

    // keine weitere Produktion, wenn input-Slot leer is
    game->update(60000, 5000);

    ASSERT_EQ(0, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(2, sheepFarm->productionSlots.output.inventory);
}