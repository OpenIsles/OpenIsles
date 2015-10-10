#include "GameTest.h"


class HarvestableTest : public GameTest {

protected:
    Harvestable* northernForest1;
    Harvestable* northernForest2;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        FourthDirection southView = Direction::SOUTH;

        northernForest1 = game->addHarvestable(
            MapCoords(30, 30), configMgr->getMapObjectType("northern-forest1"), southView);
        northernForest1->setAge(1.0);

        northernForest2 = game->addHarvestable(
            MapCoords(30, 31), configMgr->getMapObjectType("northern-forest2"), southView);
        northernForest2->setAge(1.3);
    }
};


/**
 * @brief Prüft, ob der Wald korrekt wächst.
 */
TEST_F(HarvestableTest, growHarvestable) {
    const double allowedDoubleError = 0.001;

    // Anfangsbedingungen testen
    ASSERT_NEAR(1.0, northernForest1->getAge(), allowedDoubleError);
    ASSERT_NEAR(1.3, northernForest2->getAge(), allowedDoubleError);

    // Wachsum sollte 1.0 pro 40 Sekunden Spielzeit sein.
    // Test nach 20 Sekunden: Erwarte Wachsum von 0.5
    game->update(20000);

    ASSERT_NEAR(1.5, northernForest1->getAge(), allowedDoubleError);
    ASSERT_NEAR(1.8, northernForest2->getAge(), allowedDoubleError);

    // Test nach 40 Sekunden: Erwarte Wachsum von 1.0
    game->update(20000);

    ASSERT_NEAR(2.0, northernForest1->getAge(), allowedDoubleError);
    ASSERT_NEAR(2.3, northernForest2->getAge(), allowedDoubleError);

    // Test nach 108 Sekunden: Erwarte Wachsum von 2.7
    game->update(68000);

    ASSERT_NEAR(3.7, northernForest1->getAge(), allowedDoubleError);
    ASSERT_NEAR(4.0, northernForest2->getAge(), allowedDoubleError);

    // Maximalwert ist 4,0. northernForest2 sollte nicht weiterwachsen, northernForest1 aber schon
    game->update(4000);

    ASSERT_NEAR(3.8, northernForest1->getAge(), allowedDoubleError);
    ASSERT_NEAR(4.0, northernForest2->getAge(), allowedDoubleError);

    // Maximalwert ist 4,0. Wälder müssen nun vollgewachsen sein.
    game->update(999999);

    ASSERT_NEAR(4.0, northernForest1->getAge(), allowedDoubleError);
    ASSERT_NEAR(4.0, northernForest2->getAge(), allowedDoubleError);
}