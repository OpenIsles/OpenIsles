#include "graphics/mgr/sdl/SDLFontMgr.h"
#include "graphics/renderer/sdl/SDLRenderer.h"
#include "utils/Color.h"
#include "utils/Rect.h"

SDLFontMgr::SDLFontMgr() {
    // Schriften vorladen, von denen wir wissen, dass wir sie sicher brauchen
    getFont("DroidSans-Bold.ttf", 14);
}

SDLFontMgr::~SDLFontMgr() {
    for (auto iter = ttfFonts.cbegin(); iter != ttfFonts.cend(); iter++) {
        TTF_CloseFont(iter->second);
    }
}

void SDLFontMgr::renderText(IRenderer* renderer, std::string string, int x, int y,
                            Color* color, Color* shadowColor, std::string fontName, int fontSize, int align) {
    
    TTF_Font* ttfFont = getFont(fontName, fontSize);
    SDL_Color sdlColor = { color->r, color->g, color->b, color->a };
    SDL_Surface* surfaceText = TTF_RenderUTF8_Solid(ttfFont, string.c_str(), sdlColor);
    
    Rect rectDestination;
    rectDestination.w = surfaceText->w;
    rectDestination.h = surfaceText->h;
    
    switch (align & RENDERTEXT_HALIGN_MASK) {
        default: // fall-through
        case RENDERTEXT_HALIGN_LEFT:
            rectDestination.x = x;
            break;
        case RENDERTEXT_HALIGN_RIGHT:
            rectDestination.x = x - surfaceText->w;
            break;
        case RENDERTEXT_HALIGN_CENTER:
            rectDestination.x = x - surfaceText->w / 2;
            break;
    }
    
    switch (align & RENDERTEXT_VALIGN_MASK) {
        default: // fall-through
        case RENDERTEXT_VALIGN_TOP:
            rectDestination.y = y;
            break;
        case RENDERTEXT_VALIGN_BOTTOM:
            rectDestination.y = y - surfaceText->h;
            break;
        case RENDERTEXT_VALIGN_MIDDLE:
            rectDestination.y = y - surfaceText->h / 2;
            break;
    }

    SDL_Renderer* sdlRealRenderer = (dynamic_cast<SDLRenderer*>(renderer))->getRealRenderer();
    
    if (shadowColor != nullptr) {
        Rect rectDestinationShadow(
            rectDestination.x + 1, rectDestination.y + 1, rectDestination.w, rectDestination.h);

        SDL_Color sdlShadowColor = { shadowColor->r, shadowColor->g, shadowColor->b, shadowColor->a };
        SDL_Surface* surfaceTextShadow = TTF_RenderUTF8_Solid(ttfFont, string.c_str(), sdlShadowColor);
        SDL_Texture* textureTextShadow = SDL_CreateTextureFromSurface(sdlRealRenderer, surfaceTextShadow);
        SDL_FreeSurface(surfaceTextShadow);

        SDL_Rect sdlRectDistinationShadow = {
            rectDestinationShadow.x, rectDestinationShadow.y, rectDestinationShadow.w, rectDestinationShadow.h };
        SDL_RenderCopy(sdlRealRenderer, textureTextShadow, nullptr, &sdlRectDistinationShadow);
        SDL_DestroyTexture(textureTextShadow);
    }
    
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(sdlRealRenderer, surfaceText);
	SDL_FreeSurface(surfaceText);

    SDL_Rect sdlRectDestination = {
        rectDestination.x, rectDestination.y, rectDestination.w, rectDestination.h };
	SDL_RenderCopy(sdlRealRenderer, textureText, nullptr, &sdlRectDestination);
	SDL_DestroyTexture(textureText);
}

TTF_Font* SDLFontMgr::getFont(std::string fontName, int fontSize) {
    std::pair<std::string, int> mapKey = std::pair<std::string, int>(fontName, fontSize);
    auto iter = ttfFonts.find(mapKey);
    
    // Schon geladen
    if (iter != ttfFonts.end()) {
        return iter->second;
    }
    
    // Noch nicht geladen
    std::string fontPath = "data/font/" + fontName;
    TTF_Font* ttfFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    ttfFonts[mapKey] = ttfFont;
    return ttfFont;
}