#include "GameTest.h"


/**
 * @brief Test, der sich mit Wohnhäusern beschäftigt.
 */
class HousesTest : public GameTest {

protected:
    virtual void SetUp() {
        GameTest::SetUp();
    }
};

/**
 * @brief Testet, dass neue Häuser genau einen Bewohner kriegen
 */
TEST_F(HousesTest, checkThatNewHousesHaveExactlyOneInhabitant) {
    FourthDirection southView = Direction::SOUTH;
    Player* player = game->getPlayer(0);
    const MapObjectType* pioneersHouse3 = configMgr->getMapObjectType("pioneers-house3");

    Colony* colony = game->getColony({29, 27});
    
    // Anfangsbevölkerung in der Kolonie merken
    const PopulationTier* pioneers = configMgr->getPopulationTier("pioneers");
    const PopulationTier* settlers = configMgr->getPopulationTier("settlers");
    unsigned int colonyPopulationAtStart = colony->population;
    unsigned int pioneersPopulationAtStart = colony->populationTiers[pioneers].population;
    unsigned int settlersPopulationAtStart = colony->populationTiers[settlers].population;

    // Haus bauen und checken, ob die Bevölkerung angestiegen is
    Building* newHouse = (Building*) game->addStructure({29, 27}, pioneersHouse3, southView, player);
    ASSERT_EQ(1, newHouse->inhabitants);
    ASSERT_EQ(colonyPopulationAtStart + 1, colony->population);
    ASSERT_EQ(pioneersPopulationAtStart + 1, colony->populationTiers[pioneers].population);
    ASSERT_EQ(settlersPopulationAtStart, colony->populationTiers[settlers].population);
}