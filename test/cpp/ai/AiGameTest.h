#ifndef _AI_GAME_TEST_H
#define _AI_GAME_TEST_H

#include "GameTest.h"
#include "ai/AiMgr.h"

/**
 * @brief Basisklasse für alle Test-Fixtures, die KI testen.
 */
class AiGameTest : public GameTest {

private:
    /**
     * Dateiname des KI-Scripts, das geladen werden soll.
     */
    std::string aiScriptFilename;

protected:
    AiMgr* aiMgr;

protected:
    AiGameTest() : AiGameTest("test-map.tmx", "simple-ai.lua") {}
    AiGameTest(const std::string& mapFilename, const std::string& aiScriptFilename) :
        GameTest(mapFilename), aiScriptFilename(aiScriptFilename) {}

    virtual void SetUp() {
        GameTest::SetUp();

        aiMgr = new AiMgr(context);
        aiMgr->loadScript("data/ai/" + aiScriptFilename);
        aiMgr->init();
    }

    virtual void TearDown() {
        delete aiMgr;
    }

    /**
     * @brief Führt Spiellogik mit KI aus
     *
     * Methode ist analog zu Game::update(unsigned long, unsigned long)
     *
     * @param millisecondsElapsedSum Millisekunden, die das Spiel fortschreiten soll
     * @param millisecondsIncrement größtes Zeitintervall in Millisekunden,
     *                              das für den `update()`-Aufruf verwendet werden soll
     */
    void updateWithAi(unsigned long millisecondsElapsedSum, unsigned long millisecondsIncrement) {
        unsigned long millisecondsToDoRemaining = millisecondsElapsedSum;

        while (millisecondsToDoRemaining > 0) {
            unsigned long nextMillisecondsHopp =
                (millisecondsToDoRemaining > millisecondsIncrement) ? millisecondsIncrement : millisecondsToDoRemaining;

            context.game->update(nextMillisecondsHopp);
            aiMgr->update();

            millisecondsToDoRemaining -= nextMillisecondsHopp;
        }
    }

};

#endif