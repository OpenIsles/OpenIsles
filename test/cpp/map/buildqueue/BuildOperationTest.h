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
    Colony* colony;

protected:
    BuildOperationTest() : GameTest("buildqueue-testmap.tmx") {
    }

    virtual void SetUp() {
        GameTest::SetUp();

        player = game->getCurrentPlayer();
        colony = game->getColony({43, 34});
    }

    /**
     * @brief Assert-Helper. Prüft, ob ein Map-Objekt korrekt in die Map übernommen wurde.
     * @param mapCoords erwartete Map-Koordinaten
     * @param mapObjectType erwarteter MapObjectType
     * @param view erwartete View des MapObject
     * @param mapWidth erwartete mapWidth des MapObject
     * @param mapHeight erwartete mapHeight des MapObject
     */
    void assertCorrectBuild(const MapCoords& mapCoords, const MapObjectType* mapObjectType,
                            const FourthDirection& view, unsigned char mapWidth, unsigned char mapHeight) const {

        const MapObjectFixed* mapObjectFixed = game->getMap()->getMapObjectFixedAt(mapCoords);
        ASSERT_TRUE(mapObjectFixed != nullptr);
        ASSERT_EQ(mapCoords, mapObjectFixed->getMapCoords());
        ASSERT_EQ(mapObjectType, mapObjectFixed->getMapObjectType());
        ASSERT_EQ(view, mapObjectFixed->getView());
        ASSERT_EQ(mapWidth, mapObjectFixed->getMapWidth());
        ASSERT_EQ(mapHeight, mapObjectFixed->getMapHeight());
    }

    /**
     * @brief Assert-Helper. Prüft, ob ein BuildOperationResultBit für eine Abriss-Operation korrekt gesetzt ist
     * @param resultBit Result-Bit zu prüfen
     */
    void assertCorrectResultBitForDemolish(const BuildOperationResultBit& resultBit) const {
        ASSERT_EQ(true, resultBit.deleteMapObjectThere);

        // immer false beim Abreißen
        ASSERT_EQ(false, resultBit.somethingInTheWay);
        ASSERT_EQ(false, resultBit.resourcesEnoughToBuildThis);
        ASSERT_EQ(false, resultBit.costsNothingBecauseOfChange);

        ASSERT_TRUE(resultBit.mapObjectToReplaceWith == nullptr);
        ASSERT_TRUE(resultBit.mapObjectToDraw == nullptr);
    }
};