#include <gtest/gtest.h>
#include <unordered_set>
#include "map/Building.h"
#include "utils/CatchmentAreaIterator.h"
#include "utils/RectangleData.h"


/**
 * Teste mit einem Gebäude, was keinen Einzugsbereich besitzt
 */
TEST(CatchmentAreaIteratorTest, noCatchmentArea) {
    // Testaufbau
    MapObjectType mapObjectType;
    mapObjectType.catchmentArea = nullptr;

    Building building;
    building.setMapObjectType(&mapObjectType);
    building.setMapCoords({40, 50});
    building.setView(Direction::SOUTH);

    // Testdurchführung
    CatchmentAreaIterator catchmentAreaIteratorFalse(building, false);
    catchmentAreaIteratorFalse.iterate([](const MapCoords& mapCoords) {
        FAIL(); // leerer Einzugsbereich. iterate() darf kein Mal aufgerufen werden!
    });

    CatchmentAreaIterator catchmentAreaIteratorTrue(building, true);
    catchmentAreaIteratorTrue.iterate([](const MapCoords& mapCoords) {
        FAIL(); // leerer Einzugsbereich. iterate() darf kein Mal aufgerufen werden!
    });
}


/**
 * Teste mit einem quadratischen Gebäude mit symmetrischen Einzugsbereich.
 * Beispiel wie Rinderfarm (6x6 Einzugsbereich mit 2x2 Gebäude).
 */
class CatchmentAreaIteratorWithCatchmentAreaTest : public ::testing::Test {

protected:
    MapObjectType mapObjectType;
    Building building;

public:
    CatchmentAreaIteratorWithCatchmentAreaTest() {
        // Testaufbau
        RectangleData<char>* catchmentArea = new RectangleData<char>(6, 6);
        std::strcpy(catchmentArea->data, "011110111111111111111111111111011110");

        mapObjectType.mapWidth = 2;
        mapObjectType.mapHeight = 2;
        mapObjectType.catchmentArea.reset(catchmentArea);

        building.setMapObjectType(&mapObjectType);
        building.setMapCoords({40, 50});
        building.setMapWidth(2);
        building.setMapHeight(2);
        building.setView(Direction::SOUTH);
    }

    void doTest(bool ignoreBuilding, unsigned int expectedSize, MapCoords* mapCoordsExpected) {
        // Testdurchführung
        std::unordered_set<MapCoords> mapCoordsIterated;

        CatchmentAreaIterator catchmentAreaIterator(building, ignoreBuilding);
        catchmentAreaIterator.iterate([&](const MapCoords& mapCoords) {
            mapCoordsIterated.insert(mapCoords);
        });

        // Testauswertung
        ASSERT_EQ(expectedSize, mapCoordsIterated.size());
        for (unsigned int i = 0; i < expectedSize; i++) {
            ASSERT_TRUE(mapCoordsIterated.find(mapCoordsExpected[i]) != mapCoordsIterated.cend());
        }
    }
};

TEST_F(CatchmentAreaIteratorWithCatchmentAreaTest, ignoreBuilding) {
    MapCoords mapCoordsExpected[28] = {
                  {39, 48}, {40, 48}, {41, 48}, {42, 48},
        {38, 49}, {39, 49}, {40, 49}, {41, 49}, {42, 49}, {43, 49},
        {38, 50}, {39, 50},                     {42, 50}, {43, 50},
        {38, 51}, {39, 51},                     {42, 51}, {43, 51},
        {38, 52}, {39, 52}, {40, 52}, {41, 52}, {42, 52}, {43, 52},
                  {39, 53}, {40, 53}, {41, 53}, {42, 53}
    };

    doTest(true, 28, mapCoordsExpected);
}

TEST_F(CatchmentAreaIteratorWithCatchmentAreaTest, dontIgnoreBuilding) {
    MapCoords mapCoordsExpected[32] = {
                  {39, 48}, {40, 48}, {41, 48}, {42, 48},
        {38, 49}, {39, 49}, {40, 49}, {41, 49}, {42, 49}, {43, 49},
        {38, 50}, {39, 50}, {40, 50}, {41, 50}, {42, 50}, {43, 50},
        {38, 51}, {39, 51}, {40, 51}, {41, 51}, {42, 51}, {43, 51},
        {38, 52}, {39, 52}, {40, 52}, {41, 52}, {42, 52}, {43, 52},
                  {39, 53}, {40, 53}, {41, 53}, {42, 53}
    };

    doTest(false, 32, mapCoordsExpected);
}
