#ifndef _GUI_LUA_CONSOLE_H
#define _GUI_LUA_CONSOLE_H

#include <lua.hpp>
#include <list>
#include <string>
#include "global.h"
#include "gui/components/GuiBase.h"


/**
 * @brief GUI-Element für die Lua-Konsole
 */
class GuiLuaConsole : public GuiBase {

private:
    /**
     * @brief Eingegebene, noch nicht abgeschickte Eingabe
     */
    std::string currentInput;

    /**
     * @brief Liste mit den kürzlich ausgeführten Befehlen. Es werden nun die letzten x Befehle ausgehoben.
     */
    std::list<std::string> history;

    /**
     * @brief Lua-Kontext, in dem Befehle ausgeführt werden.
     */
    lua_State* lua;

public:
    /**
     * @brief Konstruktor.
     * @param context Dependency
     */
    GuiLuaConsole(const Context& context);

    virtual ~GuiLuaConsole();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer);

    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     * @return `true` um das Event an weitere GUI-Elemente zu reichen,
     *         `false` um anzudeuten, dass das Event bereits verarbeitet wurde. Es wird dann nicht weiter zugestellt.
     */
    virtual bool onEventElement(SDL_Event& event);

private:
    /**
     * @brief Führt den aktuellen Befehl aus.
     */
    void executeCommand();
};

#endif
