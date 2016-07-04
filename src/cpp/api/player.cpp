#include "api/api-global.h"
#include "game/Game.h"
#include "game/Player.h"


DEFINE_LUA_FUNCTION(getPlayerCount) {
    lua_pushinteger(lua, Player::MAX_PLAYERS);
    return 1;
}

DEFINE_LUA_FUNCTION(getPlayer) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    int playerIndex = luaL_checkinteger(lua, -1);
    const Player* player = context->game->getPlayer(playerIndex - 1);
    if (player == nullptr || !player->isUsed()) {
        return luaL_error(lua, _("Invalid playerIndex.").c_str());
    }

    lua_createtable(lua, 0, 3);

    lua_pushnumber(lua, player->coins);
    lua_setfield(lua, -2, "coins");

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