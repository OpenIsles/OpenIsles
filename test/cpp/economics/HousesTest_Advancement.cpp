#include "GameTest.h"


/**
 * @brief Test, der prüft, ob die Wohnhäuser korrekt aufsteigen
 */
class HousesTest_Advancement : public GameTest {

protected:
    const MapObjectType* pioneersHouse1;
    const MapObjectType* pioneersHouse2;
    const MapObjectType* pioneersHouse3;

    const MapObjectType* settlersHouse1;
    const MapObjectType* settlersHouse2;
    const MapObjectType* settlersHouse3;
    const MapObjectType* settlersHouse4;
    const MapObjectType* settlersHouse5;

    const FourthDirection southView = Direction::SOUTH;
    Player* player;
    Colony* colony;

    const PopulationTier* pioneers;
    const PopulationTier* settlers;
    Building* houses[8];

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");
        pioneersHouse2 = configMgr->getMapObjectType("pioneers-house2");
        pioneersHouse3 = configMgr->getMapObjectType("pioneers-house3");

        settlersHouse1 = configMgr->getMapObjectType("settlers-house1");
        settlersHouse2 = configMgr->getMapObjectType("settlers-house2");
        settlersHouse3 = configMgr->getMapObjectType("settlers-house3");
        settlersHouse4 = configMgr->getMapObjectType("settlers-house4");
        settlersHouse5 = configMgr->getMapObjectType("settlers-house5");

        player = game->getPlayer(0);
        colony = game->getColony({29, 27});

        pioneers = configMgr->getPopulationTier("pioneers");
        settlers = configMgr->getPopulationTier("settlers");

        // 8 Häuser bauen
        houses[0] = (Building*) game->addStructure({27, 23}, pioneersHouse1, southView, player);
        houses[1] = (Building*) game->addStructure({29, 23}, pioneersHouse2, southView, player);
        houses[2] = (Building*) game->addStructure({31, 23}, pioneersHouse3, southView, player);
        houses[3] = (Building*) game->addStructure({27, 25}, pioneersHouse1, southView, player);
        houses[4] = (Building*) game->addStructure({31, 25}, pioneersHouse2, southView, player);
        houses[5] = (Building*) game->addStructure({27, 27}, pioneersHouse1, southView, player);
        houses[6] = (Building*) game->addStructure({29, 27}, pioneersHouse2, southView, player);
        houses[7] = (Building*) game->addStructure({31, 27}, pioneersHouse3, southView, player);

        // Baumaterial zur Verfügung stellen
        colony->getGoods(configMgr->getGood("tools")).inventory = 10;
        colony->getGoods(configMgr->getGood("wood")).inventory = 30;
        colony->getGoods(configMgr->getGood("bricks")).inventory = 0; // keine Ziegel, um Siedler->Bürger zu verhindern

        // Nahrung geben, damit keiner später verhungert
        colony->getGoods(configMgr->getFoodGood().good).capacity = 100;
        colony->getGoods(configMgr->getFoodGood().good).inventory = 100;
    }

    /**
     * @brief Zählt, wie viele der 8 Häuser eine bestimmte Bevölkerungsgruppe haben
     * @param populationTier Bevölkerungsgruppe
     * @return Anzahl der Häuser (0 bis 8), welche diese Bevölkerungsgruppe haben
     */
    unsigned char countHousesPerPopulationTier(const PopulationTier* populationTier) {
        unsigned char count = 0;

        for (int i = 0; i < 8; i++) {
            if (houses[i]->getMapObjectType()->populationTier == populationTier) {
                count++;
            }
        }
        return count;
    }
};

/**
 * @brief Test, der alle Häuser aufsteigen lässt
 */
TEST_F(HousesTest_Advancement, allHousesAdvancing) {
    unsigned long mapObjectsCount = game->getMap()->getMapObjects().size();

    ASSERT_EQ(8, colony->populationTiers[pioneers].population);
    ASSERT_EQ(0, colony->populationTiers[settlers].population);

    // 10 Minuten spielen, danach müssen alle umgebaut haben
    game->update(10 * TICKS_PER_MINUTE, 2000);
    ASSERT_EQ(8, countHousesPerPopulationTier(settlers));
    ASSERT_EQ(0, countHousesPerPopulationTier(pioneers));

    // Prüfe, ob alle Häuser zufällig Ansichten und MapObjectType bekommen haben
    unsigned char countPerView[Direction::MAX] = {0};
    unsigned char countSettlersHouses1 = 0;
    unsigned char countSettlersHouses2 = 0;
    unsigned char countSettlersHouses3 = 0;
    unsigned char countSettlersHouses4 = 0;
    unsigned char countSettlersHouses5 = 0;

    for (int i = 0; i < 8; i++) {
        // alle Häuser müssen Siedlerhäuser sein
        if (houses[i]->getMapObjectType() == settlersHouse1) {
            countSettlersHouses1++;
        } else if (houses[i]->getMapObjectType() == settlersHouse2) {
            countSettlersHouses2++;
        } else if (houses[i]->getMapObjectType() == settlersHouse3) {
            countSettlersHouses3++;
        } else if (houses[i]->getMapObjectType() == settlersHouse4) {
            countSettlersHouses4++;
        } else if (houses[i]->getMapObjectType() == settlersHouse5) {
            countSettlersHouses5++;
        } else {
            FAIL();
        }

        countPerView[int(houses[i]->getView())]++;
    }

    // Sicherstellen, dass die Ansichten zufällig sind
    ASSERT_NE(8, countPerView[int(southView)]);
    ASSERT_EQ(8, countPerView[int(Direction::SOUTH)] + countPerView[int(Direction::EAST)] +
                 countPerView[int(Direction::NORTH)] + countPerView[int(Direction::WEST)]);

    // Sicherstellen, dass lauter verschiedene Siedlerhäuser da sind
    ASSERT_LE(countSettlersHouses1, 5);
    ASSERT_LE(countSettlersHouses2, 5);
    ASSERT_LE(countSettlersHouses3, 5);
    ASSERT_LE(countSettlersHouses4, 5);
    ASSERT_LE(countSettlersHouses5, 5);
    ASSERT_EQ(8, countSettlersHouses1 + countSettlersHouses2 + countSettlersHouses3 +
                 countSettlersHouses4 + countSettlersHouses5);

    // MapObjects-Liste checken: Es darf nix dazugekommen sein!
    ASSERT_EQ(mapObjectsCount, game->getMap()->getMapObjects().size());
}

/**
 * @brief Test, der sicherstellt, dass nur volle Häuser umbauen
 */
TEST_F(HousesTest_Advancement, onlyFullHousesAreAdvancing) {
    ASSERT_EQ(8, colony->populationTiers[pioneers].population);

    // 10 Minuten spielen, danach müssen alle umgebaut haben
    for (;;) {
        bool allowedToAdvance[8] = {false};
        bool wasPioneersBefore[8] = {false};

        // Gucken, welche Häuser aufsteigen dürften
        for (int i = 0; i < 8; i++) {
            if (houses[i]->getMapObjectType()->populationTier == pioneers) {
                wasPioneersBefore[i] = true;
                if (houses[i]->inhabitants == 2) {
                    allowedToAdvance[i] = true;
                }
            }
        }

        // Spielen
        game->update(2000);

        // Bei einem Aufstieg checken, ob er erlaubt war
        for (int i = 0; i < 8; i++) {
            // Gerade aufgestiegen?
            if (houses[i]->getMapObjectType()->populationTier == settlers && wasPioneersBefore[i]) {
                ASSERT_TRUE(allowedToAdvance[i]);
                ASSERT_EQ(3, houses[i]->inhabitants); // Einwohnerzahl muss 2+1 sein
            }
        }

        if (countHousesPerPopulationTier(settlers) == 8) {
            break;
        }

        ASSERT_NO_ENDLESS_LOOP(10 * TICKS_PER_MINUTE, "waiting for advancement");
    }
}

/**
 * @brief Test, der das Timing überprüft
 */
TEST_F(HousesTest_Advancement, timings) {
    // Initial alle Häuser voll machen
    for (int i = 0; i < 8; i++) {
        game->addInhabitantsToBuilding(houses[i], 1);
    }

    ASSERT_EQ(0, countHousesPerPopulationTier(settlers));
    ASSERT_EQ(8, countHousesPerPopulationTier(pioneers));

    // Erster Umbau muss innerhalb von 15 Sekunden gehen
    while (countHousesPerPopulationTier(settlers) != 1) {
        game->update(1000);
        ASSERT_NO_ENDLESS_LOOP(15000, "waiting for first settlers house");
    };

    ASSERT_EQ(1, countHousesPerPopulationTier(settlers));
    ASSERT_EQ(7, countHousesPerPopulationTier(pioneers));

    // Alle weiteren Umbauten müssen im Bereich 15s-120s liegen
    // Im Schnitt 45s, d.h. für 7 Häuser etwa 315 Sekunden
    unsigned long ticksLastAdvancement = game->getTicks();
    unsigned char lastSettlerCount = 1;
    for (;;) {
        game->update(1000);

        unsigned char settlerCount = countHousesPerPopulationTier(settlers);
        if (settlerCount > lastSettlerCount + 1) {
            FAIL(); // zu viele auf einmal umgebaut
        }

        if (settlerCount == lastSettlerCount + 1) {
            // Einer umgebaut: Mindestzeit dazwischen testen
            unsigned long ticksForAdvancement = game->getTicks() - ticksLastAdvancement;
            ticksLastAdvancement = game->getTicks();

            ASSERT_GT(ticksForAdvancement, 15*TICKS_PER_SECOND);
            ASSERT_LT(ticksForAdvancement, 120*TICKS_PER_SECOND);
        }
        lastSettlerCount = settlerCount;

        if (settlerCount == 8) {
            break;
        }

        ASSERT_NO_ENDLESS_LOOP(350000, "waiting for further settlers houses");
    }
}

/**
 * @brief Häuser sind zwar aufstiegsbereit, das Baumaterial geht aber nach 2 Häusern aus
 */
TEST_F(HousesTest_Advancement, notEnoughConstructionMaterial) {
    colony->getGoods(configMgr->getGood("tools")).inventory = 2;

    ASSERT_EQ(8, colony->populationTiers[pioneers].population);
    ASSERT_EQ(0, colony->populationTiers[settlers].population);

    game->update(240000, 2000);
    ASSERT_EQ(2, countHousesPerPopulationTier(settlers));
    ASSERT_EQ(6, countHousesPerPopulationTier(pioneers));
}

/**
 * @brief Test, der checkt, ob das Baumaterial verbraucht wurde
 */
TEST_F(HousesTest_Advancement, constructionMaterialUsed) {
    while (countHousesPerPopulationTier(settlers) != 8) {
        game->update(2000);

        unsigned char settlersCount = countHousesPerPopulationTier(settlers);
        ASSERT_EQ(10 - 1*settlersCount, colony->getGoods(configMgr->getGood("tools")).inventory);
        ASSERT_EQ(30 - 3*settlersCount, colony->getGoods(configMgr->getGood("wood")).inventory);

        ASSERT_NO_ENDLESS_LOOP(350000, "waiting for settlers houses");
    }

    ASSERT_EQ(2, colony->getGoods(configMgr->getGood("tools")).inventory);
    ASSERT_EQ(6, colony->getGoods(configMgr->getGood("wood")).inventory);
}

/**
 * @brief Test, der prüft, ob beim Umbau die Einwohner korrekt zwischen den Bevölkerungsgruppen umgebucht werden
 */
TEST_F(HousesTest_Advancement, colonyPopulation) {
    ASSERT_EQ(8, colony->populationTiers[pioneers].population);
    ASSERT_EQ(0, colony->populationTiers[settlers].population);

    // Erstes Haus umbauen lassen
    game->addInhabitantsToBuilding(houses[3], 1);
    while (countHousesPerPopulationTier(settlers) != 1) {
        game->update(2000);
        ASSERT_NO_ENDLESS_LOOP(15000, "waiting for settlers house");
    };

    ASSERT_EQ(1, countHousesPerPopulationTier(settlers));
    ASSERT_EQ(7, countHousesPerPopulationTier(pioneers));

    // Kolonie-Bevölkerung checken
    ASSERT_EQ(7, colony->populationTiers[pioneers].population);
    ASSERT_GT(colony->populationTiers[settlers].population, 0);
}

/**
 * @brief Testet, wenn Baumaterial gesperrt wurde
 */
// TODO Test aktivieren, wenn Baumaterial gesperrt werden kann
TEST_F(HousesTest_Advancement, DISABLED_constructionMaterialDisallowed) {
    ASSERT_EQ(8, colony->populationTiers[pioneers].population);
    ASSERT_EQ(0, colony->populationTiers[settlers].population);

    // 10 Minuten spielen, danach darf keiner umgebaut haben
    game->update(10 * TICKS_PER_MINUTE, 2000);
    ASSERT_EQ(0, countHousesPerPopulationTier(settlers));
    ASSERT_EQ(8, countHousesPerPopulationTier(pioneers));

    // Bevölkerungswachstum muss aber funktionieren
    ASSERT_EQ(16, colony->populationTiers[pioneers].population);
}