#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <cstdint>

class ConfigMgr;
class EconomicsMgr;
class Game;
class GuiMgr;
class IFontMgr;
class IGraphicsMgr;
class IRandomEngine;
class ISoundMgr;

/**
 * @brief Diese Klasse enthält die Referenzen auf alle benötigten Mgr-Klassen. Sie wird in den meisten Konstruktoren
 * als Parameter weitergereicht.
 */
struct Context {

    Context() : mouseCurrentX(0), mouseCurrentY(0) {}


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
    IFontMgr* fontMgr;

    /**
     * @brief Grafik-Manager
     */
    IGraphicsMgr* graphicsMgr;

    /**
     * @brief die Benutzeroberfläche
     */
    GuiMgr* guiMgr;

    /**
     * @brief der Sound-Manager
     */
    ISoundMgr* soundMgr;

    /**
     * @brief Zustand des Spiels
     */
    Game* game;

    /**
     * @brief Zufallsgenerator-Engine
     */
    IRandomEngine* randomEngine;

    /**
     * @brief aktuelle Position des Mauszeigers in Fenster-Koordinaten
     */
    int mouseCurrentX, mouseCurrentY;

    /**
     * @brief erste Event-ID von SDL für unsere eigenen Events. Zu verwenden mit den `USER_EVENT_*`-Konstanten
     * (siehe `Events.h`)
     */
    uint32_t userEventBase;

};

/**
 * @brief Basisklasse für alle Klassen, die einen `Context` erhalten. Das verhindert, dass wir in jede Klasse dieselbe
 * `context`-Property hinzufügen müssen
 */
class ContextAware {

public:
    ContextAware(const Context& context) : context(context) {}

protected:
    const Context& context;

};


#endif
