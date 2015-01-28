#include <game/Colony.h>
#include "GameTest.h"


class EconomicsMgrTest : public GameTest {

protected:
    Building* cattleFarm;
    Building* office1;

protected:
    virtual void SetUp() {
        GameTest::SetUp();

        FourDirectionsView southView = FourDirectionsView("south");
        Player* player = game->getPlayer(0);

        office1 = dynamic_cast<Building*>(game->getMap()->getMapObjectAt(MapCoords(28, 18)));

        // Rinderfarm bauen und Zick-Zack-Straße zum Kontor ziehen
        game->addStructure(MapCoords(28, 20), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(28, 21), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(28, 22), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(27, 22), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(26, 22), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(26, 23), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(26, 24), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(26, 25), StructureType::STREET, southView, player);
        game->addStructure(MapCoords(26, 26), StructureType::STREET, southView, player);
        cattleFarm = dynamic_cast<Building*>(
            game->addStructure(MapCoords(26, 27), StructureType::CATTLE_FARM, southView, player));

        // Spielgeschwindigkeit auf 1x setzen. Die Tests hier sind genau getimt.
        context.game->setSpeed(1);
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

    Colony* colony = game->getColony(office1);

    // Lager voll machen
    cattleFarm->productionSlots.output.increaseInventory(999);

    // Anfangsbedingungen: kein Marktkarren da, Gebäudelager voll und bereit zur Abholung, 5t in der Kolonie
    ASSERT_EQ(5, colony->getGoods(GoodsType::CATTLE).inventory);

    ASSERT_TRUE(office1->carrier == nullptr);

    ASSERT_TRUE(cattleFarm->productionSlots.output.isInventoryFull());
    ASSERT_FALSE(cattleFarm->productionSlots.output.markedForPickup);

    // "Start": Erstes Update nach 1ms aufrufen. Danach sollte nun ein Marktkarren mit Abholauftrag da sein.
    office1->setLastUpdateTime(0);
    context.sdlTicks = 1;
    economicsMgr->update(office1);

    ASSERT_EQ(1, office1->getLastUpdateTime());
    ASSERT_TRUE(office1->carrier != nullptr);
    ASSERT_TRUE(office1->carrier->route->back() == MapCoords(26, 27));
    ASSERT_TRUE(office1->carrier->onOutboundTrip);
    ASSERT_EQ(GoodsType::CATTLE, office1->carrier->carriedGoods.goodsType);
    ASSERT_EQ(0, office1->carrier->carriedGoods.inventory);
    ASSERT_TRUE(office1->carrier->getMapCoords() == DoubleMapCoords(28, 19));

    ASSERT_TRUE(cattleFarm->productionSlots.output.markedForPickup);

    // "Step 1": Testet Bewegung innerhalb einer Kachel
    // Nach einer Sekunde: Der Marktkarren muss nun eine dreiviertelte Kachel (speed = 0.75, siehe EconomicsMgr) bewegt sein.
    context.sdlTicks += 1000; // Träger sollte sich (1000 * 0,75 =) 0,75 Kacheln fortbewegt haben
    economicsMgr->update(office1);

    ASSERT_EQ(1001, office1->getLastUpdateTime());
    ASSERT_NEAR(28, office1->carrier->getMapCoords().x(), allowedCoordsError);
    ASSERT_NEAR(19.75, office1->carrier->getMapCoords().y(), allowedCoordsError);

    // "Step 2": Test, wenn eine Kachel geradlinig übersprungen wird
    context.sdlTicks += 2500; // Träger sollte sich (2500 * 0,75 =) 1,875 Kacheln fortbewegt haben
    economicsMgr->update(office1);

    ASSERT_EQ(3501, office1->getLastUpdateTime());
    ASSERT_NEAR(28, office1->carrier->getMapCoords().x(), allowedCoordsError);
    ASSERT_NEAR(21.625, office1->carrier->getMapCoords().y(), allowedCoordsError);

    // "Step 3": Test, wenn mehrere Kacheln, auch über Ecken hinweg, übersprungen werden
    context.sdlTicks += 8000; // Träger sollte sich (8000 * 0,75 =) 6 Kacheln fortbewegt haben
    economicsMgr->update(office1);

    ASSERT_EQ(11501, office1->getLastUpdateTime());
    ASSERT_NEAR(26, office1->carrier->getMapCoords().x(), allowedCoordsError);
    ASSERT_NEAR(25.625, office1->carrier->getMapCoords().y(), allowedCoordsError);

    // Am Gebäude ankommen, sollte den Rücktransport triggern
    context.sdlTicks += 2000; // Träger sollte sich (2000 * 0,75 =) 1,5 Kacheln fortbewegt haben. Ziel wurde bereits nach 1,375 Kacheln erreicht.
    economicsMgr->update(office1);

    ASSERT_EQ(13501, office1->getLastUpdateTime());
    ASSERT_TRUE(office1->carrier != nullptr);
    ASSERT_TRUE(office1->carrier->route->back() == MapCoords(28, 19));
    ASSERT_FALSE(office1->carrier->onOutboundTrip);
    ASSERT_EQ(GoodsType::CATTLE, office1->carrier->carriedGoods.goodsType);
    ASSERT_EQ(4, office1->carrier->carriedGoods.inventory); // Rinderfarm kann nur 4 Tonnen halten
    ASSERT_TRUE(office1->carrier->getMapCoords() == DoubleMapCoords(26, 27));

    ASSERT_EQ(0, cattleFarm->productionSlots.output.inventory);
    ASSERT_FALSE(cattleFarm->productionSlots.output.markedForPickup);

    // Ein großer Zeitsprung und der Rücktransport sollte längst erledigt sein.
    context.sdlTicks += 20000;
    economicsMgr->update(office1);

    ASSERT_TRUE(office1->carrier == nullptr);
    ASSERT_EQ(9, colony->getGoods(GoodsType::CATTLE).inventory);
}