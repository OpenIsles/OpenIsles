#include "game/Colony.h"
#include "GameTest.h"


class EconomicsMgrTest : public GameTest {

protected:
    Building* cattleFarm;
    Building* office1;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        FourthDirection southView = Direction::SOUTH;
        Player* player = game->getPlayer(0);
        const MapObjectType* cobbledStreet = configMgr->getMapObjectType("cobbled-street");

        office1 = dynamic_cast<Building*>(game->getMap()->getMapObjectFixedAt(MapCoords(28, 18)));

        // Rinderfarm bauen und Zick-Zack-Straße zum Kontor ziehen
        game->addStreet(MapCoords(28, 20), cobbledStreet, southView, player, StreetConnections("0010"));
        game->addStreet(MapCoords(28, 21), cobbledStreet, southView, player, StreetConnections("1010"));
        game->addStreet(MapCoords(28, 22), cobbledStreet, southView, player, StreetConnections("1001"));
        game->addStreet(MapCoords(27, 22), cobbledStreet, southView, player, StreetConnections("0101"));
        game->addStreet(MapCoords(26, 22), cobbledStreet, southView, player, StreetConnections("0110"));
        game->addStreet(MapCoords(26, 23), cobbledStreet, southView, player, StreetConnections("1010"));
        game->addStreet(MapCoords(26, 24), cobbledStreet, southView, player, StreetConnections("1010"));
        game->addStreet(MapCoords(26, 25), cobbledStreet, southView, player, StreetConnections("1010"));
        game->addStreet(MapCoords(26, 26), cobbledStreet, southView, player, StreetConnections("1000"));
        cattleFarm = dynamic_cast<Building*>(game->addStructure(
            MapCoords(26, 27), configMgr->getMapObjectType("cattle-farm"), southView, player));
    }

};


/**
 * @brief Test, ob der Marktkarren die Route korrekt abläuft. Besonders wichtig ist der Übergang zwischen 2 Kacheln
 * Wir machen die Rinderfarm-Lager voll, damit das Kontor abholen muss und testen damit, ob der Marktkarren
 * ordnungsgemäß läuft.
 *
 * Siehe `doc/economics-mgr-test.xcf` für den Testaufbau und die einzelnen Schritte
 */
TEST_F(EconomicsMgrTest, updateCarrier) {
    // Da es keine halbe Ticks gibt, treten Rundungsfehler zwischen den echten Koordinaten und den theoretisch
    // errechneten Koordinaten auf. Dies ist der maximale Fehler, den die Tests erlauben.
    const double allowedCoordsError = 0.01;

    Colony* colony = office1->getColony();
    const Good* cattleGood = configMgr->getGood("cattle");

    // Lager voll machen
    cattleFarm->productionSlots.output.increaseInventory(999);

    // Anfangsbedingungen: kein Marktkarren da, Gebäudelager voll und bereit zur Abholung, 5t in der Kolonie
    ASSERT_EQ(5, colony->getGoods(cattleGood).inventory);

    ASSERT_TRUE(office1->carriers.empty());

    ASSERT_TRUE(cattleFarm->productionSlots.output.isInventoryFull());
    ASSERT_FALSE(cattleFarm->productionSlots.output.markedForPickup);

    // "Start": Erstes Update nach 1ms aufrufen. Danach sollte nun ein Marktkarren mit Abholauftrag da sein.
    office1->setLastUpdateTicks(0);
    game->update(1);

    ASSERT_EQ(1, office1->carriers.size());
    const Carrier* carrier = *office1->carriers.cbegin();

    ASSERT_EQ(1, office1->getLastUpdateTicks());
    ASSERT_EQ(1, carrier->getLastUpdateTicks());
    ASSERT_EQ(carrier->owningBuilding, office1);
    ASSERT_TRUE(carrier->route.back() == MapCoords(26, 27));
    ASSERT_TRUE(carrier->onOutboundTrip);
    ASSERT_EQ("cattle", carrier->carriedGoods.good->name);
    ASSERT_EQ(0, carrier->carriedGoods.inventory);
    ASSERT_TRUE(carrier->getMapCoords() == DoubleMapCoords(28, 19));
    ASSERT_TRUE(carrier->getCurrentMovingDirection() == Direction::SOUTH);

    ASSERT_TRUE(cattleFarm->productionSlots.output.markedForPickup);

    // "Step 1": Testet Bewegung innerhalb einer Kachel
    // Nach einer Sekunde: Der Marktkarren muss nun eine dreiviertelte Kachel (speed = 0.75, siehe EconomicsMgr) bewegt sein.
    game->update(1000); // Träger sollte sich (1000 * 0,75 =) 0,75 Kacheln fortbewegt haben

    ASSERT_EQ(1001, office1->getLastUpdateTicks());
    ASSERT_EQ(1001, carrier->getLastUpdateTicks());
    ASSERT_NEAR(28, carrier->getMapCoords().x(), allowedCoordsError);
    ASSERT_NEAR(19.75, carrier->getMapCoords().y(), allowedCoordsError);
    ASSERT_TRUE(carrier->getCurrentMovingDirection() == Direction::SOUTH);

    // "Step 2": Test, wenn eine Kachel geradlinig übersprungen wird
    game->update(2500); // Träger sollte sich (2500 * 0,75 =) 1,875 Kacheln fortbewegt haben

    ASSERT_EQ(3501, office1->getLastUpdateTicks());
    ASSERT_EQ(3501, carrier->getLastUpdateTicks());
    ASSERT_NEAR(28, carrier->getMapCoords().x(), allowedCoordsError);
    ASSERT_NEAR(21.625, carrier->getMapCoords().y(), allowedCoordsError);
    ASSERT_TRUE(carrier->getCurrentMovingDirection() == Direction::SOUTH);

    // "Step 3": Test, wenn mehrere Kacheln, auch über Ecken hinweg, übersprungen werden
    game->update(8000); // Träger sollte sich (8000 * 0,75 =) 6 Kacheln fortbewegt haben

    ASSERT_EQ(11501, office1->getLastUpdateTicks());
    ASSERT_EQ(11501, carrier->getLastUpdateTicks());
    ASSERT_NEAR(26, carrier->getMapCoords().x(), allowedCoordsError);
    ASSERT_NEAR(25.625, carrier->getMapCoords().y(), allowedCoordsError);
    ASSERT_TRUE(carrier->getCurrentMovingDirection() == Direction::SOUTH);

    // Am Gebäude ankommen, sollte den Rücktransport triggern
    game->update(2000); // Träger sollte sich (2000 * 0,75 =) 1,5 Kacheln fortbewegt haben. Ziel wurde bereits nach 1,375 Kacheln erreicht.

    ASSERT_EQ(1, office1->carriers.size());
    carrier = *office1->carriers.cbegin();

    ASSERT_EQ(13501, office1->getLastUpdateTicks());
    ASSERT_EQ(13501, carrier->getLastUpdateTicks());
    ASSERT_TRUE(carrier->route.back() == MapCoords(28, 19));
    ASSERT_FALSE(carrier->onOutboundTrip);
    ASSERT_EQ("cattle", carrier->carriedGoods.good->name);
    ASSERT_EQ(4, carrier->carriedGoods.inventory); // Rinderfarm kann nur 4 Tonnen halten
    ASSERT_TRUE(carrier->getMapCoords() == DoubleMapCoords(26, 27));
    ASSERT_TRUE(carrier->getCurrentMovingDirection() == Direction::NORTH);

    ASSERT_EQ(0, cattleFarm->productionSlots.output.inventory);
    ASSERT_FALSE(cattleFarm->productionSlots.output.markedForPickup);

    // Ein großer Zeitsprung und der Rücktransport sollte längst erledigt sein.
    game->update(20000);

    ASSERT_TRUE(office1->carriers.empty());
    ASSERT_EQ(9, colony->getGoods(cattleGood).inventory);
}