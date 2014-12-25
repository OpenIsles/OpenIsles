#ifndef _I_FONT_MGR_H
#define _I_FONT_MGR_H

#include <map>
#include <iostream>
#include "graphics/renderer/IRenderer.h"
#include "utils/Color.h"


#define RENDERTEXT_HALIGN_LEFT 0x00
#define RENDERTEXT_HALIGN_RIGHT 0x01
#define RENDERTEXT_HALIGN_CENTER 0x02
#define RENDERTEXT_HALIGN_MASK 0x03

#define RENDERTEXT_VALIGN_TOP 0x00
#define RENDERTEXT_VALIGN_BOTTOM 0x10
#define RENDERTEXT_VALIGN_MIDDLE 0x20
#define RENDERTEXT_VALIGN_MASK 0x30


/**
 * Interface für Font-Manager, der alle Schriftarten verwaltet und rendert
 */
class IFontMgr {

public:
    virtual ~IFontMgr() {}

    /**
     * @brief Rendert Text
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
    virtual void renderText(IRenderer* renderer, std::string string, int x, int y,
        Color* color, Color* shadowColor, std::string fontName, int fontSize, int align) = 0;

};

#endif
