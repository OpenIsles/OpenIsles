#include "api/api-global.h"


DEFINE_LUA_FUNCTION(debug) {
    const char* message = luaL_checkstring(lua, -1);
    Log::debug(message);
    return 0;
}