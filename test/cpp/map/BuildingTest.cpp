#include <cstring>
#include <gtest/gtest.h>
#include "config/ConfigMgr.h"
#include "map/Building.h"
#include "map/StructureType.h"


/**
 * @brief Fake-ConfigMgr, der immer denselben Einzugsbereich zurückgibt.
 */
class BuildingTestConfigMgr : public ConfigMgr {

private:
    BuildingConfig buildingConfig;

public:
    /**
     * @brief Konstruktor
     * @param catchmentArea Einzugsbereich
     */
    BuildingTestConfigMgr(RectangleData<char>* catchmentArea) {
        buildingConfig.catchmentArea = catchmentArea;
    }

    const BuildingConfig* getBuildingConfig(StructureType structureType) const override {
        return &buildingConfig;
    }
};


/* Fall 1: ganz normal
 *
 *   x=23
 *   |
 * □□▨□□
 * □▨▨▨□
 * ▨▨▣▨▨ — y=12
 * □▨▨▨□
 * □□▨□□
 */
TEST(BuildingTest, isInsideCatchmentArea_Normal) {
    Building building;
    building.setMapWidth(1);
    building.setMapHeight(1);
    building.setMapCoords(MapCoords(23, 12));
    building.setView(FourDirectionsView(FourDirectionsView::SOUTH));

    RectangleData<char>* catchmentArea = new RectangleData<char>(5, 5);
    memcpy(catchmentArea->data, "0010001110111110111000100", 25);
    BuildingTestConfigMgr* configMgr = new BuildingTestConfigMgr(catchmentArea);

    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(23, 12))); // Gebäude selber
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(22, 12))); // Punkte innerhalb Einzugsbereich
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(21, 12)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(23, 11)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(23, 10)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(24, 13)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(21, 10))); // Punkte außerhalb Einzugsbereich
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(22, 10)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(21, 11)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(23, 9))); // Außerhalb des Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(23, 15)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(20, 12)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(26, 12)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(0, 0)));

    delete configMgr;
}

/* Fall 2: asymmetrisch mit größerem Gebäude
 *
 *   x=11
 *   |
 * □□▨▨▨▨□□
 * □▨▨▨▨▨▨□
 * ▨▨▣▣▣▣▨▨ — y=33
 * ▨▨▣▣▣▣▨▨
 * ▨▨▣▣▣▣▨▨
 * □▨▨▨▨▨▨□
 * □□▨▨▨▨□□
 */
TEST(BuildingTest, isInsideCatchmentArea_AsymmetricLargeBuilding) {
    Building building;
    building.setMapWidth(4);
    building.setMapHeight(3);
    building.setMapCoords(MapCoords(11, 33));
    building.setView(FourDirectionsView(FourDirectionsView::SOUTH));

    RectangleData<char>* catchmentArea = new RectangleData<char>(8, 7);
    memcpy(catchmentArea->data, "00111100011111101111111111111111111111110111111000111100", 56);
    BuildingTestConfigMgr* configMgr = new BuildingTestConfigMgr(catchmentArea);

    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(11, 33))); // Gebäude selber
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(14, 35))); //
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(13, 34))); //
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(11, 31))); // Punkte innerhalb Einzugsbereich
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(15, 32)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(9, 33)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(16, 35)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(12, 37)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(9, 31))); // Punkte außerhalb Einzugsbereich
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(9, 32)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(10, 31)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(16, 37)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(8, 33))); // Außerhalb des Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(17, 33)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(14, 30)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(14, 38)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(0, 0)));

    delete configMgr;
}

/* Fall 3: prüft, ob sich alles korrekt verhält, wenn das Gebäude gedreht ist.
 *
 *    Süd                  Ost                Nord                 West
 *   =====                =====              ======               ======
 *
 *    x=88                 x=88                x=88                 x=88
 *    |                    |                   |                    |
 * □□□□□□□□□              □□□□              ▨▨▨□□□□□□              □▨▨▨
 * □▨▨▣▣▣▨▨▨ — y=42       □▨▨□              ▨▨▨▣▣▣▨▨□ — y=42       □▨▨▨
 * □▨▨▣▣▣▨▨▨              □▨▨□              ▨▨▨▣▣▣▨▨□              □▨▨▨
 * □□□□□□▨▨▨              □▣▣□ — y=42       □□□□□□□□□              □▣▣□ — y=42
 *                        □▣▣□                                     □▣▣□
 *                        □▣▣□                                     □▣▣□
 *                        ▨▨▨□                                     □▨▨□
 *                        ▨▨▨□                                     □▨▨□
 *                        ▨▨▨□                                     □□□□
 *
 * Wir prüfen immer dieselben MapCoords im Test.
 * Je nach Gebäude-View sind wir dann innerhalb ..oder außerhalb des Einzugsbereichs.
 */
TEST(BuildingTest, isInsideCatchmentArea_Views) {
    Building building;
    building.setMapCoords(MapCoords(88, 42));
    building.setView(FourDirectionsView(FourDirectionsView::SOUTH));

    RectangleData<char>* catchmentArea = new RectangleData<char>(9, 4);
    memcpy(catchmentArea->data, "000000000011111111011111111000000111", 36);
    BuildingTestConfigMgr* configMgr = new BuildingTestConfigMgr(catchmentArea);

    building.setMapWidth(3);
    building.setMapHeight(2);
    building.setView(FourDirectionsView(FourDirectionsView::SOUTH));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(88, 42))); // Mitte

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 41))); // Ecken des Quer-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 41)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 44)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 44)));

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 39)));  // Ecken des Hochkant-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 39)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 47)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 47)));

    building.setMapWidth(2);
    building.setMapHeight(3);
    building.setView(FourDirectionsView(FourDirectionsView::EAST));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(88, 42))); // Mitte

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 41))); // Ecken des Quer-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 41)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 44)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 44)));

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 39)));  // Ecken des Hochkant-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 39)));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 47)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 47)));

    building.setMapWidth(3);
    building.setMapHeight(2);
    building.setView(FourDirectionsView(FourDirectionsView::NORTH));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(88, 42))); // Mitte

    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 41))); // Ecken des Quer-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 41)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 44)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 44)));

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 39)));  // Ecken des Hochkant-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 39)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 47)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 47)));

    building.setMapWidth(2);
    building.setMapHeight(3);
    building.setView(FourDirectionsView(FourDirectionsView::WEST));
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(88, 42))); // Mitte

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 41))); // Ecken des Quer-Rechtecks
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 41)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(85, 44)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(93, 44)));

    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 39)));  // Ecken des Hochkant-Rechtecks
    ASSERT_TRUE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 39)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(87, 47)));
    ASSERT_FALSE(building.isInsideCatchmentArea(configMgr, MapCoords(90, 47)));

    delete configMgr;
}