#include "ai/AiGameTest.h"


class HelloWorldAiTest : public AiGameTest {

public:
    HelloWorldAiTest() : AiGameTest("saved-game.tmx", "hello-world.lua") {}
};


TEST_F(HelloWorldAiTest, helloWorld) {
    Map* map = game->getMap();
    MapCoords mapCoords(52, 217);

    // Checken, ob die KI funktioniert. Auf (52, 127) baut die KI einen Weg und reißt ihn später wieder ab
    ASSERT_TRUE(map->getMapObjectFixedAt(mapCoords) == nullptr);
    updateWithAi(35000, 2000);
    ASSERT_TRUE(map->getMapObjectFixedAt(mapCoords) != nullptr);
    updateWithAi(10000, 2000);
    ASSERT_TRUE(map->getMapObjectFixedAt(mapCoords) == nullptr);
}