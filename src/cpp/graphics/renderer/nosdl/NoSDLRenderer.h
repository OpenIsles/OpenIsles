#ifndef _NOSDL_RENDERER_H
#define _NOSDL_RENDERER_H

#include <iostream>
#include "graphics/renderer/IRenderer.h"
#include "utils/Color.h"
#include "utils/Rect.h"

/**
 * @brief Renderer, der für die Tests benutzt wird. Er tut nix.
 */
class NoSDLRenderer : public IRenderer {

public:
    NoSDLRenderer() {}
    virtual ~NoSDLRenderer() {}

    virtual void showWindow() {}

    virtual const int getWindowWidth() const {
        return 0;
    }
    virtual const int getWindowHeight() const {
        return 0;
    }

    virtual void setClipRect(const Rect* const rect) {}
    virtual void setDrawColor(const Color& color) {}
    virtual void setDrawBlendMode(int blendMode) {}
    virtual void drawLine(int x1, int y1, int x2, int y2) {}
    virtual void drawRect(const Rect& rect) {}
    virtual void fillRect(const Rect& rect) {}
    virtual void setHintRenderScaleQuality(const char* scaleQuality) {}
    virtual void startFrame() {}
    virtual void endFrame() {}
};

#endif
