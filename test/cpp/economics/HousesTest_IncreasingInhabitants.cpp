#include "GameTest.h"


/**
 * @brief Test, der prüft, ob die Bevölkerung in den Wohnhäusern sich korrekt verhält
 */
class HousesTest_IncreasingInhabitants : public GameTest {

protected:
    const MapObjectType* pioneersHouse1;
    const MapObjectType* pioneersHouse2;
    const MapObjectType* pioneersHouse3;

    const FourthDirection southView = Direction::SOUTH;
    Player* player;
    Colony* colony;

    const PopulationTier* pioneers;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");
        pioneersHouse2 = configMgr->getMapObjectType("pioneers-house2");
        pioneersHouse3 = configMgr->getMapObjectType("pioneers-house3");

        player = game->getPlayer(0);
        colony = game->getColony({29, 27});

        pioneers = configMgr->getPopulationTier("pioneers");

        // Baumaterial entziehen, sodass es nicht zum Aufstieg kommen kann
        colony->getGoods(configMgr->getGood("tools")).inventory = 0;
        colony->getGoods(configMgr->getGood("wood")).inventory = 0;
        colony->getGoods(configMgr->getGood("bricks")).inventory = 0;

        // Nahrung geben, damit keiner später verhungert
        colony->getGoods(configMgr->getFoodGood().good).capacity = 100;
        colony->getGoods(configMgr->getFoodGood().good).inventory = 100;
    }

    /**
     * @brief Testet, dass Häuser Einwohner hinzubekommen (nur Pionier-Häuser)
     * @param pioneerHousesCount Anzahl der Pioneer-Häuser, die gebaut werden
     */
    void testWithPioneersOnly(unsigned int pioneerHousesCount) {
        const MapObjectType* houses[3] = {
            pioneersHouse1, pioneersHouse2, pioneersHouse3
        };

        // Pionier-Häuser bauen
        unsigned int i = 0;
        for (int y = 23; i < pioneerHousesCount; y += 2) {
            for (int x = 27; x <= 41 && i < pioneerHousesCount; x += 2, i++) {
                game->addStructure({x, y}, houses[i % 3], southView, player);
            }
        }

        ASSERT_EQ(pioneerHousesCount, colony->populationTiers[pioneers].population);
        ASSERT_EQ(PopulationSatisfaction::GOOD, colony->populationTiers[pioneers].populationSatisfaction);

        // in den ersten zwei Minuten sollte nix passieren
        game->update(120000, 1000);
        ASSERT_EQ(pioneerHousesCount, colony->populationTiers[pioneers].population);

        unsigned long maxGameTime = 120000 + 7500*pioneerHousesCount;

        // Nun warten. Es sollten nun langsam die Einwohner dazukommen
        int lastPioneers = colony->populationTiers[pioneers].population;
        for(;;) {
            game->update(1000);

            ASSERT_EQ(PopulationSatisfaction::GOOD, colony->populationTiers[pioneers].populationSatisfaction);

            unsigned int currentPioneers = colony->populationTiers[pioneers].population;

            int diffSinceLastSecond = currentPioneers - lastPioneers;
            lastPioneers = currentPioneers;

            // Erwarte: maximal 2 Pioniere pro Sekunde, d.h. nicht alle auf einmal
            ASSERT_LE(diffSinceLastSecond, 2);

            // Erwarte: monoton steigend; kein Bevölkerungsrückgang
            ASSERT_GE(diffSinceLastSecond, 0);

            if (currentPioneers == pioneerHousesCount * 2) {
                break;
            }

            ASSERT_NO_ENDLESS_LOOP(maxGameTime, "waiting for further inhabitants");
        }
    }
};

TEST_F(HousesTest_IncreasingInhabitants, pioneersOnly_8houses) {
    testWithPioneersOnly(8);
}

TEST_F(HousesTest_IncreasingInhabitants, pioneersOnly_16houses) {
    testWithPioneersOnly(16);
}

TEST_F(HousesTest_IncreasingInhabitants, pioneersOnly_50houses) {
    testWithPioneersOnly(50);
}

TEST_F(HousesTest_IncreasingInhabitants, pioneersOnly_1house) {
    testWithPioneersOnly(1);
}

TEST_F(HousesTest_IncreasingInhabitants, neverExceedMaximumInhabitants) {
    // Pionier-Häuser bauen
    unsigned int i = 0;
    Building* buildings[50];
    for (int y = 23; i < 50; y += 2) {
        for (int x = 27; x <= 41 && i < 50; x += 2, i++) {
            buildings[i] = (Building*) game->addStructure({x, y}, pioneersHouse2, southView, player);
        }
    }

    // 5 Minuten spielen. Kein Haus darf überfüllt sein
    for (i = 0 ; i < 300; i++) {
        game->update(1000);

        for (int j = 0; j < 50; j++) {
            ASSERT_LE(buildings[j]->inhabitants,
                      buildings[j]->getMapObjectType()->populationTier->maxPopulationPerHouse);
        }
    }
}

// TODO Tests über mehrere Siedlungen