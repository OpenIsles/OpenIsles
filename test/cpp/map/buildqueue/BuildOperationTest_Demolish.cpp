#include <initializer_list>
#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"
#include "map/buildqueue/BuildOperationTest.h"


/**
 * @brief Testet das Abreißen eines Gebäudes
 *
 * Wir reißen das Wirtshaus auf (48, 37) ab.
 */
TEST_F(BuildOperationTest, demolishBuilding) {
    BuildingCosts resourcesBefore = BuildingCosts(player, colony, context.configMgr);

    const Map* map = context.game->getMap();
    const MapObjectFixed* tavern = map->getMapObjectFixedAt({48, 37});
    ASSERT_TRUE(tavern != nullptr);

    // Test 1: BuildOperation
    BuildOperation buildOperation(context, *player);
    buildOperation.requestDemolish(*tavern);

    ASSERT_EQ(MapObjectToBuild::DEMOLISH, buildOperation.getMapObjectsToBuildMode());
    ASSERT_EQ(colony, buildOperation.getColony());

    const BuildOperationResult& result = buildOperation.getResult();
    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(6, result.size());

    std::initializer_list<MapCoords> allMapCoordsTavern = {{48, 37}, {49, 37}, {50, 37}, {48, 38}, {49, 38}, {50, 38}};
    for (const MapCoords& mapCoords : allMapCoordsTavern) {
        const BuildOperationResultBit& resultBit = *result.at(mapCoords);

        ASSERT_EQ(true, resultBit.deleteMapObjectThere);

        // immer false beim Abreißen
        ASSERT_EQ(false, resultBit.somethingInTheWay);
        ASSERT_EQ(false, resultBit.resourcesEnoughToBuildThis);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);

        ASSERT_TRUE(resultBit.mapObjectToReplaceWith == nullptr);
        ASSERT_TRUE(resultBit.mapObjectToDraw == nullptr);
    }

    // Test 2: Ausführen
    buildOperation.doBuild();

    // Testauswertung: Gebäude muss weg sein
    ASSERT_TRUE(map->getMapObjectFixedAt({48, 37}) == nullptr);

    // Testauswertung: Abreißen darf nix gekostet haben
    BuildingCosts resourcesAfter = BuildingCosts(player, colony, context.configMgr);
    ASSERT_TRUE(resourcesAfter == resourcesBefore);
}

/**
 * @brief Testet, ob die Bevölkerung korrekt abnimmt
 *
 * Wir reißen einmal das Wirtshaus (48, 37) und ein Wohnhaus auf (50, 42) ab
 */
TEST_F(BuildOperationTest, populationVanishes) {
    // Testaufbau
    const PopulationTier* settlers = configMgr->getPopulationTier("settlers");

    const MapObjectFixed* tavern = game->getMap()->getMapObjectFixedAt({48, 37});
    const MapObjectFixed* house = game->getMap()->getMapObjectFixedAt({50, 42});

    // Test 1: Wirtshaus weg
    unsigned long playerPopulationBefore = player->playerStatus.population;
    unsigned int colonyPopulationBefore = colony->population;

    BuildOperation buildOperation(context, *player);
    buildOperation.requestDemolish(*tavern);
    buildOperation.doBuild();

    ASSERT_LT(colony->population, colonyPopulationBefore);
    ASSERT_EQ(colony->population, colonyPopulationBefore - configMgr->getMapObjectType("tavern")->inhabitants);

    ASSERT_LT(player->playerStatus.population, playerPopulationBefore);

    // Test 2: Wohnhaus weg
    playerPopulationBefore = player->playerStatus.population;
    colonyPopulationBefore = colony->population;
    unsigned int settlersBefore = colony->populationTiers[settlers].population;
    unsigned int populationInHouse = dynamic_cast<const Building*>(house)->inhabitants;

    BuildOperation buildOperation2(context, *player);
    buildOperation2.requestDemolish(*house);
    buildOperation2.doBuild();

    ASSERT_LT(colony->populationTiers[settlers].population, settlersBefore);
    ASSERT_EQ(colony->populationTiers[settlers].population, settlersBefore - populationInHouse);

    ASSERT_LT(colony->population, colonyPopulationBefore);
    ASSERT_EQ(colony->population, colonyPopulationBefore - populationInHouse);

    ASSERT_LT(player->playerStatus.population, playerPopulationBefore);
}

/**
 * @brief Testet, ob die Finanzen korrekt aktualisiert werden
 *
 * Wir reißen wieder das Wirtshaus (48, 37) und ein Wohnhaus auf (50, 42) ab
 */
TEST_F(BuildOperationTest, financesUpdate) {
    // Testaufbau
    const MapObjectFixed* tavern = game->getMap()->getMapObjectFixedAt({48, 37});
    const MapObjectFixed* house = game->getMap()->getMapObjectFixedAt({50, 42});

    // Steuersatz extrem setzen, damit wir später n Unterschied sehen, wenn ein Haus abgerissen wird
    const PopulationTier* settlers = configMgr->getPopulationTier("settlers");
    colony->populationTiers[settlers].taxesPercent = 255;
    economicsMgr->updatePlayerStatus();

    // Test 1: Wirtshaus weg
    long balanceBefore = player->playerStatus.balance;

    BuildOperation buildOperation(context, *player);
    buildOperation.requestDemolish(*tavern);
    buildOperation.doBuild();

    ASSERT_GT(player->playerStatus.balance, balanceBefore);

    // Test 2: Wohnhaus weg
    balanceBefore = player->playerStatus.balance;

    BuildOperation buildOperation2(context, *player);
    buildOperation2.requestDemolish(*house);
    buildOperation2.doBuild();

    ASSERT_LT(player->playerStatus.balance, balanceBefore);
}

// TODO Schafffarm abreißen -> Schäfchen verschwinden
// TODO Marktplatz abreißen -> Lagerbestand, Marktkarren verschwinden
// TODO Kontor abreißen -> Kolonie vernichten (erst später, wenn nur ein Kontor pro Kolonie da is)