#include "gui/FontMgr.h"

FontMgr::FontMgr() {
    // Schriften vorladen, von denen wir wissen, dass wir sie sicher brauchen
    getFont("DroidSans-Bold.ttf", 14);
}

FontMgr::~FontMgr() {
    for (auto iter = ttfFonts.cbegin(); iter != ttfFonts.cend(); iter++) {
        TTF_CloseFont(iter->second);
    }
}

void FontMgr::renderText(SDL_Renderer* renderer, std::string string, int x, int y,
                         SDL_Color* color, SDL_Color* shadowColor, std::string fontName, int fontSize, int align) {
    
    TTF_Font* ttfFont = getFont(fontName, fontSize);
    SDL_Surface* surfaceText = TTF_RenderUTF8_Solid(ttfFont, string.data(), *color);
    
    SDL_Rect rectDestination;
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
    
    if (shadowColor != nullptr) {
        SDL_Rect rectDestinationShadow = { 
            rectDestination.x + 1, rectDestination.y + 1, rectDestination.w, rectDestination.h };
        
        SDL_Surface* surfaceTextShadow = TTF_RenderUTF8_Solid(ttfFont, string.data(), *shadowColor);
        SDL_Texture* textureTextShadow = SDL_CreateTextureFromSurface(renderer, surfaceTextShadow);
        SDL_FreeSurface(surfaceTextShadow);
        SDL_RenderCopy(renderer, textureTextShadow, nullptr, &rectDestinationShadow);
        SDL_DestroyTexture(textureTextShadow);
    }
    
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
	SDL_FreeSurface(surfaceText);
	SDL_RenderCopy(renderer, textureText, nullptr, &rectDestination);
	SDL_DestroyTexture(textureText);
}

TTF_Font* FontMgr::getFont(std::string fontName, int fontSize) {
    std::pair<std::string, int> mapKey = std::pair<std::string, int>(fontName, fontSize);
    auto iter = ttfFonts.find(mapKey);
    
    // Schon geladen
    if (iter != ttfFonts.end()) {
        return iter->second;
    }
    
    // Noch nicht geladen
    std::string fontPath = "data/font/" + fontName;
    TTF_Font* ttfFont = TTF_OpenFont(fontPath.data(), fontSize);
    ttfFonts[mapKey] = ttfFont;
    return ttfFont;
}