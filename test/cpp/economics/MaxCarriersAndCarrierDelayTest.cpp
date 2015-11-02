#include <map/MapObjectType.h>
#include "GameTest.h"


class MaxCarriersAndCarrierDelayTest : public GameTest {

protected:
    Building* sheepFarm;
    Building* cattleFarm;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        sheepFarm = dynamic_cast<Building*>(game->addStructure(
            MapCoords(28, 30), configMgr->getMapObjectType("sheep-farm"),
            Direction::SOUTH, game->getPlayer(0)));
        cattleFarm = dynamic_cast<Building*>(game->addStructure(
            MapCoords(38, 28), configMgr->getMapObjectType("cattle-farm"),
            Direction::SOUTH, game->getPlayer(0)));

        // Schafe/Rinder so einstellen, dass sie ewig zum Grasfressen brauchen.
        // Wir wollen nicht, dass während des Tests welche zurücklaufen.
        configMgr->getMapObjectType("sheep")->secondsToProduce = 9999;
    }
};


/**
 * @brief Überprüft, dass jedes Gebäude seine Träger losschickt, die maximale Anzahl der Träger nicht
 * überschreitet und zwischen den einzelnen Trägern die Wartezeit einhält.
 */
TEST_F(MaxCarriersAndCarrierDelayTest, checkThatCarriersAreSentCorrectly) {
    // Checke Konfiguration
    ASSERT_EQ(3, sheepFarm->getMapObjectType()->maxCarriers);
    ASSERT_EQ(2, cattleFarm->getMapObjectType()->maxCarriers);

    // Am Anfang hat keiner einen Träger
    ASSERT_EQ(0, sheepFarm->carriers.size());
    ASSERT_EQ(0, cattleFarm->carriers.size());

    // Erster Träger kommt nach 2 Sekunden raus. Wir prüfen nach 1,5s und 2,5s Spielzeit
    game->update(1500);
    ASSERT_EQ(0, sheepFarm->carriers.size());
    ASSERT_EQ(0, cattleFarm->carriers.size());

    game->update(1000);
    ASSERT_EQ(1, sheepFarm->carriers.size());
    ASSERT_EQ(1, cattleFarm->carriers.size());

    // Zweiter Träger kommt weitere 10 Sekunden später raus. Wir prüfen 9,5s und 10,5s später
    game->update(9000);
    ASSERT_EQ(1, sheepFarm->carriers.size());
    ASSERT_EQ(1, cattleFarm->carriers.size());

    game->update(1000);
    ASSERT_EQ(2, sheepFarm->carriers.size());
    ASSERT_EQ(2, cattleFarm->carriers.size());

    // Nur die Schaffarm erlaubt drei Träger. Nochmal 10 Sekunden später. Wir prüfen wieder 9,5s und 10,5s später
    game->update(9000);
    ASSERT_EQ(2, sheepFarm->carriers.size());
    ASSERT_EQ(2, cattleFarm->carriers.size());

    game->update(1000);
    ASSERT_EQ(3, sheepFarm->carriers.size());
    ASSERT_EQ(2, cattleFarm->carriers.size());

    // Nochmal 10 Sekunden später darf keiner mehr einen weiteren Träger kriegen.
    game->update(10000);
    ASSERT_EQ(3, sheepFarm->carriers.size());
    ASSERT_EQ(2, cattleFarm->carriers.size());
}