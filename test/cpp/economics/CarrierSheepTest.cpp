#include <map/MapObjectType.h>
#include "GameTest.h"


class CarrierSheepTest : public GameTest {

protected:
    Building* sheepFarm;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        sheepFarm = dynamic_cast<Building*>(game->addStructure(
            MapCoords(35, 35), configMgr->getMapObjectType("sheep-farm"),
            Direction::SOUTH, game->getPlayer(0)));

        // Schaffarm einstellen, dass nur ein Schaf losgeschickt wird, um den Test nicht zu verkomplizieren
        configMgr->getMapObjectType("sheep-farm")->maxCarriers = 1;

        // Schafe können 2 Einheiten tragen (statt normal 4), damit der Test kürzer wird
        configMgr->getMapObjectType("sheep")->carrier.capacity = 2;
    }
};


/**
 * @brief Testet, ob ein Schaf mehrere Felder Gras erntet, bevor es wieder zu seiner Schaffarm zurückkehrt.
 */
TEST_F(CarrierSheepTest, checkThatSheepsHarvestMultipleTimesBeforeReturning) {
    ASSERT_EQ(2, configMgr->getMapObjectType("sheep")->carrier.capacity);
    ASSERT_EQ(0, sheepFarm->productionSlots.output.inventory);
    ASSERT_EQ(3, game->getMap()->getMapObjects().size()); // Schaffarm, Kontor und Marktplatz

    // Wir stellen immer alle Felder bis auf eins auf abgeerntet, um die Route deterministisch zu machen
    Map* map = game->getMap();
    for (int y = 30; y < 40; y++) {
        for (int x = 30; x < 40; x++) {
            map->getMapTileAt({x, y})->nextHarvestTicks = 999999;
        }
    }

    // Träger losschicken
    ASSERT_TRUE(sheepFarm->carriers.empty());

    map->getMapTileAt({32, 36})->nextHarvestTicks = 0;
    game->update(2500);
    ASSERT_EQ(1, sheepFarm->carriers.size());
    ASSERT_EQ(4, game->getMap()->getMapObjects().size()); // Schaf dazugekommen

    const Carrier* carrier = *sheepFarm->carriers.cbegin();
    ASSERT_EQ("grass", carrier->carriedGoods.good->name);
    ASSERT_EQ(configMgr->getMapObjectType("sheep")->carrier.capacity, carrier->carriedGoods.capacity);
    ASSERT_EQ(0, carrier->carriedGoods.inventory);
    ASSERT_EQ(Carrier::WALKING, carrier->state);
    ASSERT_EQ(MapCoords(32, 36), *carrier->route.crbegin());

    // Warten, bis der Träger die erste Kachel erreicht und geerntet hat
    while (carrier->getMapCoords() != MapCoords(32, 36)) {
        game->update(50);
        ASSERT_NO_ENDLESS_LOOP(10000, "walking to first maptile");
    }
    ASSERT_EQ(Carrier::HARVESTING, carrier->state);

    map->getMapTileAt({38, 38})->nextHarvestTicks = 0; // nächste Kachel "freischalten"
    do {
        game->update(50);
        ASSERT_NO_ENDLESS_LOOP(28000, "harvesting first maptile");
    } while(carrier->state == Carrier::HARVESTING);

    // Träger hat das erste Feld geerntet und sollte nun auf dem Weg zum zweiten Feld sein
    ASSERT_EQ("grass", carrier->carriedGoods.good->name);
    ASSERT_EQ(1, carrier->carriedGoods.inventory);
    ASSERT_GT(map->getMapTileAt({32, 36})->nextHarvestTicks, 0);

    ASSERT_EQ(Carrier::WALKING, carrier->state);
    ASSERT_EQ(MapCoords(32, 36), *carrier->route.cbegin());
    ASSERT_EQ(MapCoords(38, 38), *carrier->route.crbegin());

    // Warten, bis der Träger die zweite Kachel erreicht und geerntet hat
    while (carrier->getMapCoords() != MapCoords(38, 38)) {
        game->update(50);
        ASSERT_NO_ENDLESS_LOOP(40000, "walking to second maptile");
    }
    ASSERT_EQ(Carrier::HARVESTING, carrier->state);

    do {
        game->update(50);
        ASSERT_NO_ENDLESS_LOOP(58000, "harvesting second maptile");
    } while(carrier->state == Carrier::HARVESTING);

    // Träger sollte nun wieder auf dem Rückweg zur Schaffarm sein
    ASSERT_EQ(2, carrier->carriedGoods.inventory);
    ASSERT_GT(map->getMapTileAt({38, 38})->nextHarvestTicks, 0);

    ASSERT_EQ(Carrier::WALKING, carrier->state);
    ASSERT_EQ(MapCoords(38, 38), *carrier->route.cbegin());
    ASSERT_EQ(MapCoords(36, 36), *carrier->route.crbegin());

    do {
        game->update(50);
        ASSERT_NO_ENDLESS_LOOP(65000, "sheep returning");
    } while(sheepFarm->carriers.size() == 1);

    ASSERT_EQ(0, sheepFarm->carriers.size());
    ASSERT_EQ(1, sheepFarm->productionSlots.output.inventory); // TODO hier muss später wieder eine 2 kommen, wenn unsichtbare Güter verarbeitet werden
    ASSERT_EQ(3, game->getMap()->getMapObjects().size()); // Schaf wieder weg
}