#include "api/api-global.h"
#include "game/Game.h"
#include "game/Player.h"

static constexpr int playersCount = 4;


DEFINE_LUA_FUNCTION(getPlayerCount) {
    // TODO aktuell haben wir immer 4 Spieler. Das sollte änderbar sein

    lua_pushinteger(lua, playersCount);
    return 1;
}

DEFINE_LUA_FUNCTION(getPlayer) {
    int playerIndex = luaL_checkinteger(lua, -1);
    if (playerIndex < 0 || playerIndex > playersCount) {
        return luaL_error(lua, _("Invalid playerIndex").c_str());
    }

    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));
    const Player* player = context->game->getPlayer(playerIndex - 1);

    lua_createtable(lua, 0, 4);

    lua_pushnumber(lua, player->coins);
    lua_setfield(lua, -2, "coins");

    lua_pushinteger(lua, player->getColorIndex());
    lua_setfield(lua, -2, "color");

    if (player->isHuman()) {
        lua_pushstring(lua, "human");
    } else if (player->isAI()) {
        lua_pushstring(lua, "ai");
    } else {
        lua_pushstring(lua, "(unknown)");
        assert(false);
    }
    lua_setfield(lua, -2, "type");

    lua_pushstring(lua, player->getName().c_str());
    lua_setfield(lua, -2, "name");

    return 1;
}