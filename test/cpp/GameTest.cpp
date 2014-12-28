#include "GameTest.h"

TEST_F(GameTest, SetUp) {
    // Checken, ob die Karte und die Objekte ordentlich geladen sind
    Map* map = game->getMap();

    ASSERT_TRUE(map != nullptr);
    ASSERT_EQ(150, map->getWidth());
    ASSERT_EQ(150, map->getHeight());
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
    ASSERT_EQ(28, mapX);
    ASSERT_EQ(18, mapY);
    ASSERT_EQ(3, mapWidth);
    ASSERT_EQ(2, mapHeight);
    ASSERT_EQ(StructureType::OFFICE1, ((Building*) mapObjectOffice1)->getStructureType());

    Player* playerOffice1 = mapObjectOffice1->getPlayer();
    ASSERT_EQ(mapTileOnOffice1->player, playerOffice1);
    ASSERT_EQ(PlayerColor::RED, playerOffice1->getColor());
    ASSERT_EQ("Spieler 1", playerOffice1->getName());
}