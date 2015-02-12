#include <gtest/gtest.h>
#include "map/Directions.h"
#include "map/Map.h"


/**
 * @brief Fake-Objekt, was eine 10x10-Kacheln große Map vorgaukelt, die auf MapCoords(0, 0) zentriert ist,
 * damit wir die Koordinatenumrechnung testen können.
 */
class TestMap : public Map {

public:
    TestMap(Context const* const context) : Map(context) {
        setMapCoordsCentered(MapCoords(0, 0));
    }

    int getWidth() const {
        return 10;
    }
    int getHeight() const {
        return 10;
    }

};


class MapCoordUtilsTest : public ::testing::Test {

protected:
    const Context* context;
    TestMap* map;

    static const int countTestCases = 7 * 4;
    struct {
        MapCoords mapCoords;
        ScreenCoords screenCoords;
        FourDirectionsView view;
    } testCases[countTestCases] = {
        { MapCoords(0, 0), ScreenCoords(0, 0), FourDirectionsView("south") },
        { MapCoords(1, 0), ScreenCoords(32, 16), FourDirectionsView("south") },
        { MapCoords(0, 1), ScreenCoords(-32, 16), FourDirectionsView("south") },
        { MapCoords(1, 1), ScreenCoords(0, 32), FourDirectionsView("south") },
        { MapCoords(9, 9), ScreenCoords(0, 288), FourDirectionsView("south") },
        { MapCoords(0, 9), ScreenCoords(-288, 144), FourDirectionsView("south") },
        { MapCoords(9, 0), ScreenCoords(288, 144), FourDirectionsView("south") },

        { MapCoords(0, 0), ScreenCoords(0, 0), FourDirectionsView("east") },
        { MapCoords(1, 0), ScreenCoords(-32, 16), FourDirectionsView("east") },
        { MapCoords(0, 1), ScreenCoords(-32, -16), FourDirectionsView("east") },
        { MapCoords(1, 1), ScreenCoords(-64, 0), FourDirectionsView("east") },
        { MapCoords(9, 9), ScreenCoords(-576, 0), FourDirectionsView("east") },
        { MapCoords(0, 9), ScreenCoords(-288, -144), FourDirectionsView("east") },
        { MapCoords(9, 0), ScreenCoords(-288, 144), FourDirectionsView("east") },

        { MapCoords(0, 0), ScreenCoords(0, 0), FourDirectionsView("north") },
        { MapCoords(1, 0), ScreenCoords(-32, -16), FourDirectionsView("north") },
        { MapCoords(0, 1), ScreenCoords(32, -16), FourDirectionsView("north") },
        { MapCoords(1, 1), ScreenCoords(0, -32), FourDirectionsView("north") },
        { MapCoords(9, 9), ScreenCoords(0, -288), FourDirectionsView("north") },
        { MapCoords(0, 9), ScreenCoords(288, -144), FourDirectionsView("north") },
        { MapCoords(9, 0), ScreenCoords(-288, -144), FourDirectionsView("north") },

        { MapCoords(0, 0), ScreenCoords(0, 0), FourDirectionsView("west") },
        { MapCoords(1, 0), ScreenCoords(32, -16), FourDirectionsView("west") },
        { MapCoords(0, 1), ScreenCoords(32, 16), FourDirectionsView("west") },
        { MapCoords(1, 1), ScreenCoords(64, 0), FourDirectionsView("west") },
        { MapCoords(9, 9), ScreenCoords(576, 0), FourDirectionsView("west") },
        { MapCoords(0, 9), ScreenCoords(288, 144), FourDirectionsView("west") },
        { MapCoords(9, 0), ScreenCoords(288, -144), FourDirectionsView("west") }
    };

protected:
    virtual void SetUp() {
        context = new Context();
        map = new TestMap(context);
    }

    virtual void TearDown() {
        delete map;
        delete context;
    }

};

TEST_F(MapCoordUtilsTest, mapToScreenCoords) {
    for (int i = 0; i < countTestCases; i++) {
        ScreenCoords actual = MapCoordUtils::mapToScreenCoords(testCases[i].mapCoords, testCases[i].view, *map);
        ASSERT_EQ(testCases[i].screenCoords, actual);
    }
}

TEST_F(MapCoordUtilsTest, screenToMapCoords) {
    for (int i = 0; i < countTestCases; i++) {
        MapCoords actual = MapCoordUtils::screenToMapCoords(testCases[i].screenCoords, testCases[i].view, *map);
        ASSERT_EQ(testCases[i].mapCoords, actual);
    }
}