#ifndef _GUI_STATIC_TEXT_ELEMENT_H
#define _GUI_STATIC_TEXT_ELEMENT_H

#include <SDL.h>
#include <string>
#include "Context.h"
#include "gui/components/GuiBase.h"
#include "utils/Color.h"


/**
 * Einfaches GUI-Element, was nur aus einer Zeile Text besteht und nix tut
 */
class GuiStaticTextElement : public GuiBase {

protected:
    std::string text;         ///< zu rendernder Text
    const Color* color;       ///< Textfarbe
    const Color* shadowColor; ///< Schattenfarbe (optional)
    std::string fontName;     ///< Dateiname der Schriftart, die verwendet werden soll
    int fontSize;             ///< Größe der Schriftart
    int align;                ///< Ausrichtung des Texts. Eine Kombination aus RENDERTEXT_?ALIGN_*-Konstanten

public:
    GuiStaticTextElement(const Context* const context);
    virtual ~GuiStaticTextElement() override;

    void setText(const std::string& text) {
        this->text = text;
    }

    void setColor(const Color* color) {
        this->color = color;
    }

    void setShadowColor(const Color* shadowColor) {
        this->shadowColor = shadowColor;
    }

    void setFontName(const std::string& fontName) {
        this->fontName = fontName;
    }

    void setFontSize(int fontSize) {
        this->fontSize = fontSize;
    }

    void setAlign(int align) {
        this->align = align;
    }

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override;

};

#endif
