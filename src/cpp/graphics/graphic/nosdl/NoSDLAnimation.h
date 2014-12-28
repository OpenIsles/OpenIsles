#ifndef _NOSDL_ANIMATION_H
#define _NOSDL_ANIMATION_H

#include "graphics/graphic/IAnimation.h"
#include "graphics/graphic/nosdl/NoSDLMapObjectGraphic.h"

/**
 * @brief Animationsgrafik - No-SDL-Implementierung für Tests
 */
class NoSDLAnimation : public virtual NoSDLMapObjectGraphic, public virtual IAnimation {

public:
    NoSDLAnimation(IRenderer* const renderer, const char* filename, unsigned char mapWidth, unsigned char mapHeight,
                   unsigned int framesCount, double fps) :
        NoSDLGraphic(renderer, filename),
        NoSDLPlainGraphic(renderer, filename),
        AbstractMapObjectGraphic(mapWidth, mapHeight),
        NoSDLMapObjectGraphic(renderer, filename, mapWidth, mapHeight) {}

    virtual ~NoSDLAnimation() {}


    virtual int getWidth() const override {
        return 0;
    }

    virtual unsigned int getFramesCount() const {
        return 0;
    }

    virtual double getFps() const {
        return 0;
    }

    virtual void drawFrameScaledAt(int x, int y, double scale, unsigned int frame) {}

};

#endif
