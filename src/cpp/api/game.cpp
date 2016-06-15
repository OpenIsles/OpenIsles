#include "api/api-global.h"
#include "game/Game.h"


DEFINE_LUA_FUNCTION(getGameTicks) {
    const Context* context = *static_cast<const Context**>(lua_getextraspace(lua));

    lua_pushnumber(lua, context->game->getTicks());
    return 1;
}