#include "GameTest.h"


class PlayerStatusTest : public GameTest {

protected:
    virtual void SetUp() {
        GameTest::SetUp();
    }
};

/**
 * @brief Testet, dass die PlayerStatus-Struktur nach dem Laden eines Spiels sofort befüllt ist (und nicht erst
 * einen Zyklus später, wenn die Finanzen aktualisiert werden)
 */
TEST_F(PlayerStatusTest, checkThatPlayerStatusIsSetRightAfterGameIsLoaded) {
    const PlayerStatus& playerStatus = game->getPlayer(0)->playerStatus;

    ASSERT_GT(playerStatus.population, 0);

    // TODO später checken, dass die Betriebskosten da sind (Steuererträge hat der Testcase keine)
//    ASSERT_GT(playerStatus.taxesIncome, 0);
}