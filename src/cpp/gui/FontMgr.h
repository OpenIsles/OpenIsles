#ifndef _FONT_MGR_H
#define _FONT_MGR_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <iostream>


#define RENDERTEXT_HALIGN_LEFT 0x00
#define RENDERTEXT_HALIGN_RIGHT 0x01
#define RENDERTEXT_HALIGN_CENTER 0x02
#define RENDERTEXT_HALIGN_MASK 0x03

#define RENDERTEXT_VALIGN_TOP 0x00
#define RENDERTEXT_VALIGN_BOTTOM 0x10
#define RENDERTEXT_VALIGN_MIDDLE 0x20
#define RENDERTEXT_VALIGN_MASK 0x30


/**
 * Font-Manager, verwaltet alle Schriftarten
 */
class FontMgr {

private: 
    /**
     * @brief Verwaltet alle geladenen Schriftarten. Map von (fontName, fontSize)-Paaren, die die geladene Schriftart
     * zuordnet.
     */
    std::map<std::pair<std::string, int>, TTF_Font*> ttfFonts;

public:
	FontMgr();
	~FontMgr();

    /**
     * Rendert Text
     * 
     * @param renderer SDL-Renderer, auf den gezeichnet werden soll
     * @param string Zeichenkette, die gezeichnet werden soll
     * @param x x-Koordinate, wo der Text platziert werden soll
     * @param y y-Koordinate, wo der Text platziert werden soll
     * @param color SDL-Farbe, mit der der Text gezeichnet werden soll
     * @param color SDL-Farbe, mit der der Schatten gezeichnet werden soll; oder nullptr, wenn kein Schatten gewünscht
     * @param fontName Dateiname der Schriftart, die verwendet werden soll (keinen Pfad angeben!), z.B. "DroidSans-Bold.ttf"
     * @param fontSize Größe der Schriftart
     * @param align Ausrichtung des Texts. Eine Kombination aus RENDERTEXT_?ALIGN_*-Konstanten
     */
    void renderText(SDL_Renderer* renderer, std::string string, int x, int y,
                    SDL_Color* color, SDL_Color* shadowColor, std::string fontName, int fontSize, int align);
    
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
