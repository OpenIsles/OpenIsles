#ifdef WINDOWS
#include <cstring>
#endif
#include <SDL.h>
#include "game/GoodsSlot.h"
#include "graphics/mgr/sdl/SDLGraphicsMgr.h"
#include "map/Structure.h"


SDLGraphicsMgr::SDLGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr) :
    AbstractGraphicsMgr(renderer, configMgr) {
}