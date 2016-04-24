#ifndef _GUI_STATIC_GRAPHIC_ELEMENT_H
#define _GUI_STATIC_GRAPHIC_ELEMENT_H

#include <SDL.h>
#include "global.h"
#include "graphics/graphic/IGraphic.h"
#include "gui/components/GuiBase.h"


/**
 * Einfaches GUI-Element, was nur aus einer Grafik besteht und nix tut
 */
class GuiStaticGraphicElement : public GuiBase {

protected:
    /**
     * @brief Grafik, die für das Element gerendert wird. Kann nullptr sein, wenn keine Grafik gezeichnet werden soll.
     */
    const IGraphic* graphic = nullptr;

public:
    GuiStaticGraphicElement(const Context& context);
    virtual ~GuiStaticGraphicElement() override;

    const IGraphic* getGraphic() const {
        return graphic;
    }

    /**
     * Setzt die Grafik für das Element.
     * @param graphic Grafik, die verwendet werden soll
     */
    void setGraphic(const IGraphic* graphic) {
        this->graphic = graphic;

        // Wenn noch keine Größe für dieses Element gesetzt ist,
        // wird das Element automatisch auf die Größe der Grafik vergrößert
        if (width == 0 || height == 0) {
            width = graphic->getWidth();
            height = graphic->getHeight();
        }
    }
    
    /**
     * @brief Zeichnet das Element (ohne Kinder)
     */
    virtual void renderElement(IRenderer* renderer) override;

};

#endif
