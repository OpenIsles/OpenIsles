#ifndef _GUI_STATIC_ELEMENT_H
#define _GUI_STATIC_ELEMENT_H

#include <SDL.h>
#include "Context.h"
#include "graphics/graphic/IGraphic.h"
#include "gui/components/GuiBase.h"


/**
 * Einfaches GUI-Element, was nur aus einer Grafik besteht und nix tut
 */
class GuiStaticElement : public GuiBase {

protected: 
    /**
     * @brief Grafik, die für das Element gerendert wird. Kann nullptr sein, wenn keine Grafik gezeichnet werden soll.
     */
    const IGraphic* graphic = nullptr;

public:
	GuiStaticElement(const Context* const context);
	virtual ~GuiStaticElement() override;

    const IGraphic* getGraphic() const {
        return graphic;
    }

    /**
     * Setzt die Grafik für das Element.
     * @param graphic Grafik, die verwendet werden soll
     */
    void setGraphic(const IGraphic* graphic) {
        this->graphic = graphic;
    }
    
	/**
	 * @brief Zeichnet das Element (ohne Kinder)
	 */
	virtual void renderElement(IRenderer* renderer) override;

};

#endif
