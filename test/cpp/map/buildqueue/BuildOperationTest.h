#include "GameTest.h"
#include "map/buildqueue/BuildOperation.h"


/**
 * @brief Test-Fixture, um die BuildOperations zu testen.
 *
 * Testaufbau:
 * @image html buildoperation-test.png
 */
class BuildOperationTest : public GameTest {

protected:
    Player* player;

protected:
    BuildOperationTest() : GameTest("buildqueue-testmap.tmx") {
    }

    virtual void SetUp() {
        GameTest::SetUp();

        player = game->getCurrentPlayer();
    }
};