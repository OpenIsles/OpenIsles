#include <ctime>
#include <stdexcept>
#include "ai/AiMgr.h"
#include "utils/Lua.h"


AiMgr::AiMgr(const Context& context) : ContextAware(context) {
    lua = createNewLuaState(context);
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