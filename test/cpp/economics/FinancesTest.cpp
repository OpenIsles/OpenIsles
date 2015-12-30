#include "GameTest.h"


/**
 * @brief Test, der die Finanzberechnungen abprüft
 */
class FinancesTest : public GameTest {

protected:
    const MapObjectType* pioneersHouse1;
    const MapObjectType* pioneersHouse2;
    const MapObjectType* settlersHouse1;
    const MapObjectType* settlersHouse4;

    const PopulationTier* pioneers;
    const PopulationTier* settlers;

    Colony* colony;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        pioneersHouse1 = configMgr->getMapObjectType("pioneers-house1");
        pioneersHouse2 = configMgr->getMapObjectType("pioneers-house2");
        settlersHouse1 = configMgr->getMapObjectType("settlers-house1");
        settlersHouse4 = configMgr->getMapObjectType("settlers-house4");

        pioneers = configMgr->getPopulationTier("pioneers");
        settlers = configMgr->getPopulationTier("settlers");

        // Die einzelnen Tests setzen Häuser. Zu Beginn sicherstellen, dass keine Häuser da sind
        colony = game->getColony({30, 30});
        ASSERT_EQ(0, colony->populationTiers.at(pioneers).population);
        ASSERT_EQ(0, colony->populationTiers.at(settlers).population);

        // TODO später verhindern, dass Einwohner dazukommen. Wir wollen konstante Einwohnerzahlen für diesen Test
    }
};

/**
 * @brief Test, der nur Steuereinnahmen prüft. Dieser Test stellt vorher die Betriebskosten ab.
 */
class TaxesOnlyFinancesTest : public FinancesTest {

protected:
    virtual void SetUp() override {
        FinancesTest::SetUp();

        // Betriebskosten abstellen, wir testen nur Einnahmen
        configMgr->getMapObjectType("office1")->operatingCosts.running = 0;
        configMgr->getMapObjectType("marketplace")->operatingCosts.running = 0;
        economicsMgr->updatePlayerStatus();

        Player* player = game->getPlayer(0);
        ASSERT_EQ(0, player->playerStatus.operatingCosts);
    }
};


/**
 * @brief Testet, dass Betriebskosten zum richtigen Zeitpunkt abgezogen werden
 */
TEST_F(FinancesTest, checkThatOperatingCostsWorksAtRightTimings) {
    Player* player = game->getPlayer(0);
    ASSERT_EQ(0, player->playerStatus.taxesIncome);
    ASSERT_EQ(25, player->playerStatus.operatingCosts); // Marktplatz und Kontor I
    ASSERT_EQ(-25, player->playerStatus.balance);

    // Testdurchführung
    long coinsAtStart = player->coins;

    // 9 Sekunden später darf nix passiert sein. Betriebskosten kommen nur alle 10s
    game->update(9000, 1000);
    ASSERT_EQ(coinsAtStart, player->coins);

    // Eine Sekunde später müssen die Betriebskosten abgezogen sein:
    // 25 Münzen pro Minute -> /6 -> 4 Münzen pro Zyklus
    game->update(1000);
    ASSERT_EQ(coinsAtStart - 4, player->coins);

    // Nochmal ein Interval testen
    game->update(9000, 1000);
    ASSERT_EQ(coinsAtStart - 4, player->coins);

    game->update(1000);
    ASSERT_EQ(coinsAtStart - 8, player->coins);

    // Eine ganze Minute verstreichen lassen
    game->update(60000, 5000);
    ASSERT_EQ(coinsAtStart - 32, player->coins);
}

/**
 * @brief Testet, dass Steuereinnahmen und Betriebskosten gleichzeitig funktionieren
 */
TEST_F(FinancesTest, checkThatTaxesAndOperationCostsWorkTogether) {
    Player* player = game->getPlayer(0);

    // Testaufbau: 5 Wohnhäuser setzen
    for (int x = 22; x < 32; x += 2) {
        game->addStructure({x, 22}, settlersHouse1, Direction::SOUTH, player);
    }
    ASSERT_EQ(5, colony->populationTiers.at(settlers).population);

    // 5 Siedler (Satz 1,6) mit default 32% Steuern -> 5 * 1,6 * (32%/34%) = 7,529411765 --FLOOR--> 7
    ASSERT_EQ(7, player->playerStatus.taxesIncome);

    // Marktplatz und Kontor I
    ASSERT_EQ(25, player->playerStatus.operatingCosts);

    // Bilanz
    ASSERT_EQ(-18, player->playerStatus.balance);
}

/**
 * @brief Testet, dass die initialen Steuerprozente für alle Bevölkerungsgruppen auf 32% eingestellt ist.
 */
TEST_F(TaxesOnlyFinancesTest, checkThatInitialTaxesAreSetTo32) {
    for (const PopulationTier& populationTier : configMgr->getAllPopulationTiers()) {
        ASSERT_EQ(32, colony->populationTiers.at(&populationTier).taxesPercent);
    }
}

/**
 * @brief Testet, dass Steuereinnahmen zum richtigen Zeitpunkt gutgeschrieben werden
 */
TEST_F(TaxesOnlyFinancesTest, checkThatTaxesIncomeWorksAtRightTimings) {
    Player* player = game->getPlayer(0);
    ASSERT_EQ(0, player->playerStatus.taxesIncome);

    // 40% Steuern bei den Pionieren einstellen
    colony->populationTiers.at(pioneers).taxesPercent = 38;

    // Testaufbau: 10 Wohnhäuser setzen
    for (int x = 22; x < 32; x += 2) {
        game->addStructure({x, 22}, pioneersHouse1, Direction::SOUTH, player);
        game->addStructure({x, 24}, pioneersHouse2, Direction::SOUTH, player);
    }
    ASSERT_EQ(10, colony->populationTiers.at(pioneers).population);
    ASSERT_EQ(0, colony->populationTiers.at(settlers).population);

    // Testdurchführung
    long coinsAtStart = player->coins;

    // PlayerStatus muss sofort verfügbar sein
    ASSERT_EQ(15, player->playerStatus.taxesIncome);
    ASSERT_EQ(0, player->playerStatus.operatingCosts);
    ASSERT_EQ(15, player->playerStatus.balance);

    // 9 Sekunden später darf nix passiert sein. Steuereinnahmen kommen nur alle 10s
    game->update(9000, 1000);
    ASSERT_EQ(coinsAtStart, player->coins);

    // Eine Sekunde später müssen die Steuern dazugekommen sein:
    // 10 Pioniere (Satz 1,4) mit 38% Steuern -> 10 * 1,4 * (38%/34%) = 15,647058824 --FLOOR--> 15
    // 15 Münzen pro Minute -> /6 -> 2 Münzen pro Zyklus
    game->update(1000);
    ASSERT_EQ(coinsAtStart + 2, player->coins);

    // Nochmal ein Interval testen
    game->update(9000, 1000);
    ASSERT_EQ(coinsAtStart + 2, player->coins);

    game->update(1000);
    ASSERT_EQ(coinsAtStart + 4, player->coins);

    // Eine ganze Minute verstreichen lassen
    game->update(60000, 5000);
    ASSERT_EQ(coinsAtStart + 16, player->coins);
}

/**
 * @brief Testet, dass Steuereinnahmen mit verschiedenen Bevölkerungsgruppen funktionieren
 */
TEST_F(TaxesOnlyFinancesTest, checkThatTaxesIncomeWorksWithDifferentPopulationTiers) {
    Player* player = game->getPlayer(0);
    Building* house;

    // Unterschiedliche Steuersätze einstellen: 47% bei den Pionieren, 36% bei den Siedlern
    colony->populationTiers.at(pioneers).taxesPercent = 47;
    colony->populationTiers.at(settlers).taxesPercent = 36;

    // Testaufbau: 10 Wohnhäuser setzen - 4 Pioniere, 6 Siedler mit jeweils verschiedenen Einwohnerzahlen
    // Pioniere: 2, 1, 1, 2 - Siedler: 3, 6, 4, 1, 5, 2
    house = (Building*) game->addStructure({22, 22}, pioneersHouse1, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 1);
    house = (Building*) game->addStructure({24, 22}, pioneersHouse1, Direction::SOUTH, player);
    house = (Building*) game->addStructure({26, 22}, pioneersHouse2, Direction::SOUTH, player);
    house = (Building*) game->addStructure({28, 22}, pioneersHouse2, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 1);

    house = (Building*) game->addStructure({30, 22}, settlersHouse1, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 2);
    house = (Building*) game->addStructure({22, 24}, settlersHouse1, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 5);
    house = (Building*) game->addStructure({24, 24}, settlersHouse1, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 3);
    house = (Building*) game->addStructure({26, 24}, settlersHouse4, Direction::SOUTH, player);
    house = (Building*) game->addStructure({28, 24}, settlersHouse4, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 4);
    house = (Building*) game->addStructure({30, 24}, settlersHouse4, Direction::SOUTH, player);
    game->addInhabitantsToBuilding(house, 1);

    ASSERT_EQ(6, colony->populationTiers.at(pioneers).population);
    ASSERT_EQ(21, colony->populationTiers.at(settlers).population);

    // Testdurchführung
    long coinsAtStart = player->coins;

    ASSERT_EQ(46, player->playerStatus.taxesIncome);
    ASSERT_EQ(0, player->playerStatus.operatingCosts);
    ASSERT_EQ(46, player->playerStatus.balance);

    // 6 Pioniere (Satz 1,4) mit 47% Steuer -> 6 * 1,4 * (47%/34%) = 11,611764706 --FLOOR--> 11
    // 21 Pioniere (Satz 1,6) mit 36% Steuer -> 21 * 1,6 * (36%/34%) = 35,576470588 --FLOOR--> 35
    // Erwarte 11+35 = 46 Münzen Einnahmen, d.h. 7 pro Zyklus
    game->update(10000, 1000);
    ASSERT_EQ(coinsAtStart + 7, player->coins);
}

// TODO später noch einen Test mit mehreren Spielern hinzufügen
// TODO später noch einen Test mit mehreren Kolonien auf einem Spieler hinzufügen