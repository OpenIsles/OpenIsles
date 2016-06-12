#include <fstream>
#include <sstream>
#include <stdexcept>
#include <lauxlib.h>
#include "ai/AiMgr.h"


/**
 * @brief Makro, um eine Funktion in Lua zur Verfügung zu stellen.
 * Das Makro generiert den Funktionskopf für die Funktion
 * @param name Name, unter dem die Funktion in Lua zur Verfügung stehen soll
 */
#define DEFINE_LUA_FUNCTION(name) static int LUA_ ## name(lua_State* lua)

/**
 * @brief Makro, dass eine zuvor mit DEFINE_LUA_FUNCTION definierte Funktion im `lua_state* lua` als
 * globale Funktion zur Verfügung stellt
 */
#define REGISTER_GLOBAL_LUA_FUNCTION(name) \
    lua_pushcfunction(lua, LUA_ ## name); \
    lua_setglobal(lua, #name)


DEFINE_LUA_FUNCTION(debug) {
    const char* message = lua_tostring(lua, 1);
    Log::debug(message);
    return 0;
}


AiMgr::AiMgr(const Context& context) : ContextAware(context) {
    lua = luaL_newstate();

    // API registrieren
    REGISTER_GLOBAL_LUA_FUNCTION(debug);

    // Script laden
    const char* scriptFilename = "data/ai/hello-world.lua";
    if (luaL_dofile(lua, scriptFilename)) {
        Log::error(_("Could not load AI script '%s': %s"), scriptFilename, lua_tostring(lua, -1)); // TODO Fehlermeldungen aus Lua sind nicht übersetzt
        throw std::runtime_error("Could not load AI script");
    }
}

AiMgr::~AiMgr() {
    lua_close(lua);
}

void AiMgr::update() {
    lua_getglobal(lua, "main");
    if (lua_pcall(lua, 0, 0, 0) != LUA_OK) {
        Log::error(_("Error executing AI main() function: %s"), lua_tostring(lua, -1)); // TODO Fehlermeldungen aus Lua sind nicht übersetzt
    }
}