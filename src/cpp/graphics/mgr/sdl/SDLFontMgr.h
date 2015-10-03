#ifndef _SDL_FONT_MGR_H
#define _SDL_FONT_MGR_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <iostream>
#include "graphics/mgr/IFontMgr.h"
#include "graphics/renderer/IRenderer.h"
#include "utils/Color.h"


/**
 * Font-Manager, verwaltet alle Schriftarten
 */
class SDLFontMgr : public IFontMgr {

private: 
    /**
     * @brief Verwaltet alle geladenen Schriftarten. Map von (fontName, fontSize)-Paaren, die die geladene Schriftart
     * zuordnet.
     */
    std::map<std::pair<std::string, int>, TTF_Font*> ttfFonts;

public:
    SDLFontMgr();
    ~SDLFontMgr();

    /**
     * Rendert Text
     * 
     * @param renderer Renderer, auf den gezeichnet werden soll
     * @param string Zeichenkette, die gezeichnet werden soll
     * @param x x-Koordinate, wo der Text platziert werden soll
     * @param y y-Koordinate, wo der Text platziert werden soll
     * @param color Farbe, mit der der Text gezeichnet werden soll
     * @param shadowColor Farbe, mit der der Schatten gezeichnet werden soll; oder nullptr, wenn kein Schatten gewünscht
     * @param fontName Dateiname der Schriftart, die verwendet werden soll (keinen Pfad angeben!), z.B. "DroidSans-Bold.ttf"
     * @param fontSize Größe der Schriftart
     * @param align Ausrichtung des Texts. Eine Kombination aus RENDERTEXT_?ALIGN_*-Konstanten
     */
    void renderText(IRenderer* renderer, std::string string, int x, int y,
                    const Color* color, const Color* shadowColor, std::string fontName, int fontSize, int align);
    
private:
    /**
     * Liefert die Referenz auf die Schriftart aus der Map zurück.
     * Ist die Schrift noch nicht geladen, wird sie vorher geladen und in die Map gelegt.
     * 
     * @param fontName Dateiname der Schriftart
     * @param fontSize Größe der Schriftart
     * @return Zeiger auf SDL-TTL_Font
     */
    TTF_Font* getFont(std::string fontName, int fontSize);

};

#endif
