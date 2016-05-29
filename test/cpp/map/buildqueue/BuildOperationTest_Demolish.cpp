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
        assertCorrectResultBitForDemolish(resultBit);
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

/**
 * @brief Testet das Abreißen mehrere Objekte auf einmal ab
 *
 * Wir reißen das Rechteck (41, 35)-(42, 37) ab. 2 Plätze, 2 Straßen und 2 Tabakfelder
 */
TEST_F(BuildOperationTest, demolishMultipleObjects) {
    std::initializer_list<MapCoords> mapCoordsToDemolish = {{41, 35}, {41, 36}, {41, 37}, {42, 35}, {42, 36}, {42, 37}};
    Map* map = game->getMap();

    // Test 1: BuildOperation
    BuildOperation buildOperation(context, *player);
    for (const MapCoords& mapCoords : mapCoordsToDemolish) {
        buildOperation.requestDemolish(*map->getMapObjectFixedAt(mapCoords));
    }

    ASSERT_EQ(MapObjectToBuild::DEMOLISH, buildOperation.getMapObjectsToBuildMode());
    ASSERT_EQ(colony, buildOperation.getColony());

    const BuildOperationResult& result = buildOperation.getResult();
    ASSERT_EQ(BuildOperationResult::OK, result.result);
    ASSERT_EQ(6, result.size());

    for (const MapCoords& mapCoords : mapCoordsToDemolish) {
        const BuildOperationResultBit& resultBit = *result.at(mapCoords);
        assertCorrectResultBitForDemolish(resultBit);
    }

    // Test 2: Ausführen
    buildOperation.doBuild();

    // Alle Felder müssen weg sein
    for (const MapCoords& mapCoords : mapCoordsToDemolish) {
        ASSERT_TRUE(map->getMapObjectFixedAt(mapCoords) == nullptr);
    }
}

/**
 * @brief Reißt die Schaffarm ab und prüft, ob die Schäfchen verschwinden.
 *
 * Anno 1602 lässt in diesem Fall die Schäfchen noch weiter bestehen. Sie verschwinden erst, wenn sie am Zielort
 * ankommen. Dieses Verhalten triggert auch den bekannten Gold-Bug (wo man an Stelle der Schaffarm schnell eine
 * Goldschmiede baut. Die Schäfchen liefern dann kein Gras in die Schaffarm, sondern werden in der Goldschmiede zu
 * Gold).
 *
 * Da wir uns keine Koordinaten merken, sondern eine direkte Referenz auf das Gebäude, müssen wir mit dem Vernichten
 * des Gebäudes auch alle seine Träger vernichten.
 */
TEST_F(BuildOperationTest, demolishSheepFarm_testSheepsVanish) {
    Map* map = game->getMap();
    const Building* sheepFarm = dynamic_cast<Building*>(map->getMapObjectFixedAt({34, 40}));

    // Spiel kurz laufen lassen, um die Schäfchen loszuschicken
    game->update(3000, 1000);
    ASSERT_FALSE(sheepFarm->carriers.empty());

    // Schaffarm nun abreißen
    BuildOperation buildOperation(context, *player);
    buildOperation.requestDemolish(*sheepFarm);
    buildOperation.doBuild();

    // Referenz sheepFarm und sheepFarm->carriers nun ungültig. Wir durchsuchen die MapObject-Liste manuell.
    // Finden wir einen Träger, stimmt was nicht!
    for (const MapObject* mapObject : map->getMapObjects()) {
        const Carrier* carrier = dynamic_cast<const Carrier*>(mapObject);
        ASSERT_TRUE(carrier == nullptr);
    }
}

// TODO Marktplatz abreißen -> Lagerbestand, Marktkarren verschwinden,
//                             bebaubaren Bereich einschränken, Gebäude außerhalb verfallen
// TODO Kontor abreißen -> Kolonie vernichten (erst später, wenn nur ein Kontor pro Kolonie da is)