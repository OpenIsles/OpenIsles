#ifndef _GUI_STATUS_BAR_H
#define _GUI_STATUS_BAR_H

#include <SDL_events.h>
#include <string>
#include "Context.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/components/GuiStaticTextElement.h"


/**
 * @brief GUI-Element, was die Statusleiste darstellt.
 */
class GuiStatusBar : public GuiStaticGraphicElement {

private:
    /**
     * @brief Hintergrund-Grafik
     */
    IGraphic* backgroundGraphic;

    /**
     * @brief Textelement, dass den Statusleisten-Text darstellt
     */
    GuiStaticTextElement textElement;

public:
    /**
     * @brief Konstruktor.
     * @param context Dependency
     */
    GuiStatusBar(const Context& context);

    virtual ~GuiStatusBar() override;

    /**
     * @brief Ändert den anzuzeigenden Text
     * @param text anzuzeigender Text
     */
    void setText(const std::string& text);
};

#endif
