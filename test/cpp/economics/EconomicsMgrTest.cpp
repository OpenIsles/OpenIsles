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
    // Waren anliefern, danach beginnt das Gebäude zu produzieren
    sheepFarm->productionSlots.input.inventory = 8;
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);

    game->update(1000);

    // Produktion nach 30 Sekunden abgeschlossen
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

/**
 * @brief Prüft, dass die Schaffarm erst dann Wolle produziert, nachdem das Gras angeliefert wurde,
 * selbst wenn die Schaffarm vorher die 30s Produktionszeit nix tat.
 */
TEST_F(EconomicsMgrTest, checkThatProductionStartsWithInputDelivery) {
    // Keine Input-Waren da, aber Zeit verstreichen lassen
    sheepFarm->productionSlots.input.inventory = 0;
    game->update(35000, 1000);

    ASSERT_EQ(0, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);

    // Nun Waren anliefern, Produktion darf nicht instant fertig sein
    sheepFarm->productionSlots.input.inventory = 4;
    game->update(29000, 1000);

    ASSERT_EQ(4, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);

    // Produktion jetzt
    game->update(2000, 1000);

    ASSERT_EQ(0, sheepFarm->productionSlots.input.inventory);
    ASSERT_EQ(1, sheepFarm->productionSlots.output.inventory);
}