#ifndef _NOSDL_FONT_MGR_H
#define _NOSDL_FONT_MGR_H

#include "graphics/mgr/IFontMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "utils/Color.h"


/**
 * Font-Manager für die Tests. Tut nix.
 */
class NoSDLFontMgr : public IFontMgr {

public:
    NoSDLFontMgr() {}
    ~NoSDLFontMgr() {}

    void renderText(IRenderer* renderer, std::string string, int x, int y,
                    const Color* color, const Color* shadowColor, std::string fontName, int fontSize, int align) {}

};

#endif
