#ifndef _GUI_PANEL_HEADER_H
#define _GUI_PANEL_HEADER_H

#include <SDL_render.h>
#include <string>
#include "Context.h"
#include "game/Player.h"
#include "graphics/graphic/IGraphic.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/components/GuiStaticTextElement.h"


/**
 * @brief GUI-Element, was eine Kopfzeile im Panel darstellt.
 * Es kann der Text und das Icon auf der rechten Seite angepasst werden.
 */
class GuiPanelHeader : public GuiStaticGraphicElement {

private:
    /**
     * @brief Textelement, dass den Text darstellt
     */
    GuiStaticTextElement* textElement;

    /**
     * @brief Grafik, die auf der rechten Seite angezeigt werden soll (`nullptr` = keine Grafik anzeigen)
     */
    const IGraphic* iconGraphic = nullptr;

public:
    /**
     * @brief Konstruktor. Positioniert die Komponente entsprechend `Consts::minimapClipRect`.
     * @param context Dependency
     */
    GuiPanelHeader(const Context* const context);

    virtual ~GuiPanelHeader();

    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    void renderElement(IRenderer* renderer) override;

    /**
     * @brief Ändert den anzuzeigenden Text
     * @param text anzuzeigender Text
     */
    void setText(const std::string& text);

    /**
     * @brief Ändert das Icon auf der rechten Seite
     * @param iconGraphic anzuzeigendes Icon (kann auch `nullptr` sein, um kein Icon zu verwenden)
     */
    void setIconGraphic(const IGraphic* iconGraphic) {
        this->iconGraphic = iconGraphic;
    }

    /**
     * @brief Ändert das Icon auf der rechten Seite zur einem Spieler-Wappen
     * @param player Spieler, dessen Wappen als Icon benutzt werden soll
     */
    void setCoatOfArmsIcon(const Player* player);
};

#endif
