#include <ctime>
#include <stdexcept>
#include "ai/AiMgr.h"
#include "api/api-global.h"


static const luaL_Reg apiMethods[] = {
    { "getGameTicks"  , LUA_getGameTicks },
    { "getMap"        , LUA_getMap },
    { "getIsles"      , LUA_getIsles },
    { "getShips"      , LUA_getShips },
    { "getObjects"    , LUA_getObjects },
    { "build"         , LUA_build },
    { "demolish"      , LUA_demolish },
    { "debug"         , LUA_debug },
    { "getPlayerCount", LUA_getPlayerCount },
    { "getPlayer"     , LUA_getPlayer },
    { nullptr         , nullptr }
};


AiMgr::AiMgr(const Context& context) : ContextAware(context) {
    lua = luaL_newstate();

    // Zeiger auf den Context im State sichern, damit wir später wieder rankommen
    *static_cast<const Context**>(lua_getextraspace(lua)) = &context;

    // Lua-Standard-Library
    // TODO alle Funktionen aussortieren, die wir nicht wollen
    luaL_openlibs(lua);

    // Zufallsgenerator initialisieren
    std::time_t currentTime = std::time(nullptr);
    Log::info("Initializing AI with random seed " + toString(currentTime));

    std::string luaScript = "math.randomseed(" + toString(currentTime) + ")";
    luaL_dostring(lua, luaScript.c_str());

    // API registrieren
    lua_newtable(lua);
    luaL_setfuncs(lua, apiMethods, 0);
    lua_setglobal(lua, "OpenIsles");
}

AiMgr::~AiMgr() {
    lua_close(lua);
}

void AiMgr::loadScript(const std::string& scriptFilename) {
    if (luaL_dofile(lua, scriptFilename.c_str())) {
        Log::error(_("Could not load AI script '%s': %s"), scriptFilename.c_str(), lua_tostring(lua, -1));
        throw std::runtime_error("Could not load AI script");
    }
}

void AiMgr::init() {
    lua_getglobal(lua, "init");
    if (lua_pcall(lua, 0, 0, 0) != LUA_OK) {
        Log::error(_("Error executing AI %s function: %s"), "init", lua_tostring(lua, -1));
    }
}

void AiMgr::update() {
    lua_getglobal(lua, "main");
    if (lua_pcall(lua, 0, 0, 0) != LUA_OK) {
        Log::error(_("Error executing AI %s function: %s"), "main", lua_tostring(lua, -1));
    }
}