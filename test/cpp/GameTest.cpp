#include "GameTest.h"

TEST_F(GameTest, SetUp) {
    // Checken, ob die Karte und die Objekte ordentlich geladen sind
    Map* map = game->getMap();

    ASSERT_TRUE(map != nullptr);
    ASSERT_EQ(map->getWidth(), 150);
    ASSERT_EQ(map->getHeight(), 150);
    ASSERT_FALSE(map->getMapObjects().empty());

    // Kachel, wo keine Insel ist
    MapTile* mapTileAtSea = map->getMapTileAt(92, 8);

    ASSERT_TRUE(mapTileAtSea->isle == nullptr);

    // Kachel, wo sich ein Kontor I befinden sollte
    MapTile* mapTileOnOffice1 = map->getMapTileAt(29, 19);
    ASSERT_TRUE(mapTileOnOffice1->isle != nullptr);

    MapObject* mapObjectOffice1 = mapTileOnOffice1->mapObject;
    int mapX, mapY, mapWidth, mapHeight;
    mapObjectOffice1->getMapCoords(mapX, mapY, mapWidth, mapHeight);

    ASSERT_TRUE(mapObjectOffice1 != nullptr);
    ASSERT_EQ(mapX, 28);
    ASSERT_EQ(mapY, 18);
    ASSERT_EQ(mapWidth, 3);
    ASSERT_EQ(mapHeight, 2);
    ASSERT_EQ(((Building*) mapObjectOffice1)->getStructureType(), StructureType::OFFICE1);

    Player* playerOffice1 = mapObjectOffice1->getPlayer();
    ASSERT_EQ(mapTileOnOffice1->player, playerOffice1);
    ASSERT_EQ(playerOffice1->getColor(), PlayerColor::RED);
    ASSERT_STREQ(playerOffice1->getName(), "Spieler 1");
}