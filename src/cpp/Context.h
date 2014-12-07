#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <SDL.h>

class ConfigMgr;
class EconomicsMgr;
class FontMgr;
class Game;
class GraphicsMgr;
class GuiMgr;
class SoundMgr;

/**
 * @brief Diese Klasse enthält die Referenzen auf alle benötigten Mgr-Klassen. Sie wird in den meisten Konstruktoren
 * als Parameter weitergereicht.
 */
struct Context {

    Context() : sdlTicks(0), mouseCurrentX(0), mouseCurrentY(0) {}


    /**
     * @brief Konfigurations-Manager
     */
    ConfigMgr* configMgr;

    /**
     * @brief Manager zuständig für die Wirtschaft
     */
    EconomicsMgr* economicsMgr;

    /**
     * @brief Verwaltung der Schriftarten
     */
    FontMgr* fontMgr;

    /**
     * @brief Grafik-Manager
     */
    GraphicsMgr* graphicsMgr;

    /**
     * @brief die Benutzeroberfläche
     */
    GuiMgr* guiMgr;

    /**
     * @brief der Sound-Manager
     */
    SoundMgr* soundMgr;

    /**
     * @brief Zustand des Spiels
     */
    Game* game;

    /**
     * @brief pro Frame wird einmal auf die Uhr geguckt. In dieser Variable steht der SDL-Tickcount.
     */
    Uint32 sdlTicks;

    /**
     * @brief aktuelle Position des Mauszeigers in Fenster-Koordinaten
     */
    int mouseCurrentX, mouseCurrentY;

};

/**
 * @brief Basisklasse für alle Klassen, die einen `Context` erhalten. Das verhindert, dass wir in jede Klasse dieselbe
 * `context`-Property hinzufügen müssen
 */
class ContextAware {

public:
    ContextAware(const Context* const context) : context(context) {}

protected:
    const Context* const context;

};


#endif
