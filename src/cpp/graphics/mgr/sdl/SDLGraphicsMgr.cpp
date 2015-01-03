#ifdef WINDOWS
#include <cstring>
#endif

#include "graphics/mgr/sdl/SDLGraphicsMgr.h"


SDLGraphicsMgr::SDLGraphicsMgr(IRenderer* const renderer, const ConfigMgr* const configMgr) :
    AbstractGraphicsMgr(renderer, configMgr) {
}