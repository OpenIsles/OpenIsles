#include "GameTest.h"


/**
 * @brief Test, der den Nahrungsmittelverbrauch überprüft
 */
class FoodConsumptionTest : public GameTest {

protected:
    const Good* food;

    const MapObjectType* pioneersHouse1;
    const MapObjectType* settlersHouse1;

    const PopulationTier* pioneers;
    const PopulationTier* settlers;

    Colony* colony;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        food = configMgr->getGood("food");

        pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");
        settlersHouse1 = configMgr->getMapObjectType("settlers-house1");

        pioneers = configMgr->getPopulationTier("pioneers");
        settlers = configMgr->getPopulationTier("settlers");

        // Die einzelnen Tests setzen Häuser. Zu Beginn sicherstellen, dass keine Häuser da sind
        colony = game->getColony({30, 30});
        ASSERT_EQ(0, colony->populationTiers.at(pioneers).population);
        ASSERT_EQ(0, colony->populationTiers.at(settlers).population);
        
        // FÜr alle Tests die Nahrungsbestände auf Maximum setzen
        GoodsSlot& foodGoodSlot = colony->getGoods(food);
        foodGoodSlot.increaseInventory(9999);

        ASSERT_TRUE(foodGoodSlot.isInventoryFull());
        ASSERT_EQ(40, colony->getGoods(food).inventory);


        // TODO später verhindern, dass Einwohner dazukommen. Wir wollen konstante Einwohnerzahlen für diesen Test
    }
};


/**
 * @brief Testet, dass keine Nahrung verbraucht wird, wenn keine Häuser da sind
 */
TEST_F(FoodConsumptionTest, checkThatNoFoodInConsumedIfThereAreNoHouse) {
    // Kontor und Marktplatz sind da, aber keine Häuser
    ASSERT_GT(colony->population, 0);

    // Nahrung setzen, Selbst nach 2 Minuten Spielzeit darf sich am Ende nix verändert haben
    game->update(120000, 2000);
    ASSERT_TRUE(colony->getGoods(food).isInventoryFull());
}

/**
 * @brief Testet, der Nahrungsverbrauch grundsätzlich funktioniert.
 */
TEST_F(FoodConsumptionTest, checkThatFoodInConsumed) {
    // Wir legen 20 Siedler-Häuser mit je 5 Einwohner an.
    Player* player = game->getPlayer(0);

    for (int y = 24; y < 34; y += 2) {
        for (int x = 24; x < 32; x += 2) {
            Building* building = (Building*) game->addStructure({x, y}, settlersHouse1, Direction::SOUTH, player);
            game->addInhabitantsToBuilding(building, 4);
        }
    }
    ASSERT_EQ(100, colony->populationTiers.at(settlers).population);

    // Kein Verbrauch vor einem Zyklus
    game->update(9000, 1000);
    ASSERT_EQ(40, colony->getGoods(food).inventory);

    // erster Zyklus durch: nun prüfen, ob die Siedler die Nahrung auch verbraucht haben
    game->update(2000, 1000);
    ASSERT_EQ(39, colony->getGoods(food).inventory);

    // nach 10 Zyklen: Verbrauch von 1,3 pro Zyklus, d.h. 13t
    game->update(90000, 2500);
    ASSERT_EQ(27, colony->getGoods(food).inventory);
}

/**
 * @brief Testet, der Nahrungsverbrauch über alle Bevölkerungsgruppen geht
 */
TEST_F(FoodConsumptionTest, checkThatFoodInConsumedOverAllPopulationTiers) {
    // Wir legen 20 Pioniere (10*2 Einwohner) und 20 Siedler (4*5 Einwohner) an.
    Player* player = game->getPlayer(0);

    for (int y = 24; y < 28; y += 2) {
        for (int x = 24; x < 34; x += 2) {
            Building* building = (Building*) game->addStructure({x, y}, pioneersHouse1, Direction::SOUTH, player);
            game->addInhabitantsToBuilding(building, 1);
        }
    }
    for (int x = 24; x < 32; x += 2) {
        Building* building = (Building*) game->addStructure({x, 30}, settlersHouse1, Direction::SOUTH, player);
        game->addInhabitantsToBuilding(building, 4);
    }
    ASSERT_EQ(20, colony->populationTiers.at(pioneers).population);
    ASSERT_EQ(20, colony->populationTiers.at(settlers).population);

    // Verbrauch 1,3*40/100 = 0,52t pro Zyklus. D.h. nach 10 Zyklen sollten 5t verbraucht sein.
    game->update(101000, 2500);
    ASSERT_EQ(35, colony->getGoods(food).inventory);
}