#include <ctime>
#include <string>
#include "api/api-global.h"
#include "utils/Lua.h"
#include "utils/StringFormat.h"


const luaL_Reg apiMethods[] = {
    { "getGameTicks"       , LUA_getGameTicks },
    { "getMap"             , LUA_getMap },
    { "getIsles"           , LUA_getIsles },
    { "getShips"           , LUA_getShips },
    { "getMapObjectsFixed" , LUA_getMapObjectsFixed },
    { "getMapObjectFixedAt", LUA_getMapObjectFixedAt },
    { "getColonyAt"        , LUA_getColonyAt },
    { "build"              , LUA_build },
    { "demolish"           , LUA_demolish },
    { "debug"              , LUA_debug },
    { "getPlayerCount"     , LUA_getPlayerCount },
    { "getPlayer"          , LUA_getPlayer },
    { nullptr              , nullptr }
};


lua_State* createNewLuaState(const Context& context) {
    lua_State* lua = luaL_newstate();

    // Zeiger auf den Context im State sichern, damit wir später wieder rankommen
    *static_cast<const Context**>(lua_getextraspace(lua)) = &context;

    // Lua-Standard-Library
    // TODO alle Funktionen aussortieren, die wir nicht wollen
    luaL_openlibs(lua);

    // Zufallsgenerator initialisieren
    std::time_t currentTime = std::time(nullptr);
    Log::info("Initializing new Lua state with random seed " + toString(currentTime));

    std::string luaScript = "math.randomseed(" + toString(currentTime) + ")";
    luaL_dostring(lua, luaScript.c_str());

    // API registrieren
    lua_newtable(lua);
    luaL_setfuncs(lua, apiMethods, 0);
    lua_setglobal(lua, "OpenIsles");

    return lua;
}