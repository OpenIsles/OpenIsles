#include "GameTest.h"
#include "map/coords/MapCoords.h"

TEST_F(GameTest, SetUp) {
    // Checken, ob die Karte und die Objekte ordentlich geladen sind
    Map* map = game->getMap();

    ASSERT_TRUE(map != nullptr);
    ASSERT_EQ(150, map->getWidth());
    ASSERT_EQ(150, map->getHeight());
    ASSERT_FALSE(map->getMapObjects().empty());

    // Kachel, wo keine Insel ist
    MapTile* mapTileAtSea = map->getMapTileAt(MapCoords(92, 8));

    ASSERT_TRUE(mapTileAtSea->isle == nullptr);

    // Kachel, wo sich ein Kontor I befinden sollte
    MapTile* mapTileOnOffice1 = map->getMapTileAt(MapCoords(29, 19));
    ASSERT_TRUE(mapTileOnOffice1->isle != nullptr);

    MapObjectFixed* mapObjectOffice1 = mapTileOnOffice1->mapObjectFixed;

    ASSERT_TRUE(mapObjectOffice1 != nullptr);
    ASSERT_TRUE(mapObjectOffice1->getMapCoords() == MapCoords(28, 18));
    ASSERT_EQ(3, mapObjectOffice1->getMapWidth());
    ASSERT_EQ(2, mapObjectOffice1->getMapHeight());
    ASSERT_EQ(MapObjectType::OFFICE1, ((Building*) mapObjectOffice1)->getMapObjectType());

    Player* playerOffice1 = mapObjectOffice1->getPlayer();
    ASSERT_EQ(mapTileOnOffice1->player, playerOffice1);
    ASSERT_EQ(PlayerColor::RED, playerOffice1->getColor());
    ASSERT_EQ("Spieler 1", playerOffice1->getName());
}